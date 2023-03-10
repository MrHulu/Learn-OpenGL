#include "client.h"
#include <stdexcept>
#include <iostream>
#include <functional>

SimpleClient::SimpleClient()
{
  coap_startup();
  m_ctx = coap_new_context(nullptr);
  if(!m_ctx) {
    coap_cleanup();
    throw std::runtime_error("coap_new_context fail");
  }
  
  // 网络
  coap_address_init(&m_address);
  m_address.addr.sin.sin_family = AF_INET;
  m_address.addr.sin.sin_addr.s_addr = INADDR_ANY;
  m_address.addr.sin.sin_port = htons(5683);

  // UDP方式
  m_session = coap_new_client_session(m_ctx, nullptr, &m_address, COAP_PROTO_UDP);
  if (!m_session) {
    coap_free_context(m_ctx);
    coap_cleanup();
    throw std::runtime_error("coap_new_client_session fail");
  }
}

SimpleClient::~SimpleClient()
{
  stopCoapProcess();
  coap_session_release(m_session);
  coap_free_context(m_ctx);
  coap_cleanup();
}

void SimpleClient::asynchronousStartCoapProcess(int timeout_ms)
{
  stopCoapProcess();
  std::lock_guard<std::mutex> lock(m_mutex);
  m_flag = true;
  auto lambda = [this, timeout_ms]() { this->synchronousStartCoapProcess(timeout_ms); };
  m_thread = new std::thread(lambda, this);
}

void SimpleClient::synchronousStartCoapProcess(int timeout_ms)
{
  stopCoapProcess();
  auto wait_ms = timeout_ms;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_flag = true;
  }
  coap_context_t *ctx;
  while (true) { 
    bool flag; 
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      flag = m_flag;
      ctx = m_ctx;
    }
    if (!flag) break;
    auto result = coap_io_process(ctx, wait_ms);
    if (result < 0)  
      break; 
    else if (result && result < wait_ms)
      wait_ms -= result; 
    else {
      std::lock_guard<std::mutex> lock(m_mutex);
      wait_ms = timeout_ms;
    }
  }
}

void SimpleClient::stopCoapProcess()
{
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_flag = false;
  }
  if(m_thread) {
    m_thread->join();
    delete m_thread;
    m_thread = nullptr;
  }
}

void SimpleClient::registerEvent(std::function<coap_event_handler_t> handlerFunc) noexcept
{
  coap_register_event_handler(m_ctx, *handlerFunc.target<coap_event_handler_t>());
}

void SimpleClient::registerNack(std::function<coap_nack_handler_t> handlerFunc) noexcept
{
  coap_register_nack_handler(m_ctx, *handlerFunc.target<coap_nack_handler_t>());
}

void SimpleClient::registerResponse(std::function<coap_response_handler_t> handlerFunc) noexcept
{
  coap_register_response_handler(m_ctx, *handlerFunc.target<coap_response_handler_t>());
}

void SimpleClient::sendRequest(coap_pdu_t *pdu, CallbackFunc callback)
{
  auto mid = coap_send(m_session, pdu);
  if (mid == COAP_INVALID_MID) {
    coap_show_pdu(COAP_LOG_WARN, pdu);
    throw std::runtime_error("Failed to send CoAP PDU");
  }
  m_requestsCallbackFuncs[mid] = callback;
}

void SimpleClient::observe(const std::string &urlPath, CallbackFunc callback)
{
  auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, m_session);
  
  // 添加观察者选项
  uint8_t buf[4];
  auto len = coap_encode_var_safe(buf, sizeof(buf), COAP_OBSERVE_ESTABLISH);
  coap_add_option(pdu, COAP_OPTION_OBSERVE, len, buf);

  // 解释URL
  coap_uri_t url;
  auto urlPath_const_uint8_t = reinterpret_cast<const uint8_t*>(urlPath.c_str());
  if(coap_split_uri(urlPath_const_uint8_t, urlPath.length(), &url) < 0) {
    std::string error = __FUNCTION__ + std::string("coap_split_uri is fail! path is:") +  urlPath;
    throw std::runtime_error(error.c_str());
  }
  // 解释Path
  uint8_t* segmentBuff = nullptr;
  auto segmentCount = coap_split_path(url.path.s, url.path.length, segmentBuff, nullptr);
  if (segmentCount > 0 && segmentBuff) {
      for (auto i = 0; i < segmentCount; ++i) {
        coap_add_option(pdu, COAP_OPTION_URI_PATH, urlPath.size(), (const uint8_t*)segmentBuff[i]);
      }
      coap_free(segmentBuff);
  }else{
    std::string error = __FUNCTION__ + std::string("coap_split_path is fail! path is:") +  urlPath;
    throw std::runtime_error(error.c_str());
  }
  
  // 发送
  auto mid = coap_send(m_session, pdu);
  if (mid == COAP_INVALID_MID) {
    coap_show_pdu(COAP_LOG_WARN, pdu);
    throw std::runtime_error("Failed to send CoAP PDU with observe");
  }
  m_subscriptionsCallbackFuncs[urlPath] = callback;
}

void SimpleClient::unsubscribe(const std::string &urlPath, CallbackFunc callback)
{

}

void SimpleClient::registerDefaultResponse() noexcept
{
  coap_register_response_handler(m_ctx,  defaultResponse);
}

coap_response_t SimpleClient::defaultResponse(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, coap_mid_t mid)
{
  // 获取接收的code： 402..502等
  coap_pdu_code_t rcv_code = coap_pdu_get_code(received);
  // 获取接收的类型：CON, NON, ACK, RST四个
  coap_pdu_type_t rcv_type = coap_pdu_get_type(received);
  // 获取token: TODO: 客户端需要吗？
  coap_bin_const_t token = coap_pdu_get_token(received);

  /*
    COAP_RESPONSE_CLASS(rcv_code)：
    0 空报文
    2 正确响应
    4 客服端错误
    5 服务器错误

    rcv_code & 0x1F:
    1 GET
    2 POST
    3 PUT
    4 DELETE
  */ 
  // 表示服务器返回了一个RST
  if (rcv_type == COAP_MESSAGE_RST) {
    coap_log_info("got RST\n");
    return COAP_RESPONSE_OK;
  }

  if(auto iter = m_requestsCallbackFuncs.find(mid); 
    iter != m_requestsCallbackFuncs.end()) 
  {
    iter->second(sent, received);
    m_requestsCallbackFuncs.erase(iter);
  }

  auto iter = std::find_if(m_subscriptionsCallbackFuncs.begin(), m_subscriptionsCallbackFuncs.end(),
                            [mid, received](const auto& pair) {
                              std::string urlPath((const char*)coap_get_uri_path(received)->s, coap_get_uri_path(received)->length);
                              return pair.first == urlPath;
                            });
  if (iter != m_subscriptionsCallbackFuncs.end()) {
      iter->second(sent, received);
  }
  return COAP_RESPONSE_OK;
}
