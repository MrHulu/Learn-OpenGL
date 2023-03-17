#include "client.h"
#include "utils.h"

#include <stdexcept>
#include <iostream>

std::map<coap_mid_t, SimpleClient::CallbackFunc> SimpleClient::m_requestsCallbackFuncs;
std::map<SimpleClient::subscriptionkey, SimpleClient::CallbackFunc, SimpleClient::SortByString> SimpleClient::m_subscriptionsCallbackFuncs;

SimpleClient::SimpleClient()
{
  m_requestsCallbackFuncs = std::map<coap_mid_t, CallbackFunc>{};
  m_subscriptionsCallbackFuncs = std::map<subscriptionkey, CallbackFunc, SortByString>{};
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

void SimpleClient::asynchronousStartCoapProcess(int timeout_ms) noexcept
{
  stopCoapProcess();
  std::lock_guard<std::mutex> lock(m_mutex);
  m_flag = true;
  m_thread = new std::thread(&SimpleClient::synchronousStartCoapProcess, this, timeout_ms);
}

void SimpleClient::synchronousStartCoapProcess(int timeout_ms) noexcept
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
 
void SimpleClient::stopCoapProcess() noexcept
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

void SimpleClient::registerEvent(coap_event_handler_t handlerFunc) noexcept
{
  coap_register_event_handler(m_ctx, handlerFunc);
}

void SimpleClient::registerNack(coap_nack_handler_t handlerFunc) noexcept
{
  coap_register_nack_handler(m_ctx, handlerFunc);
}

void SimpleClient::registerResponse(coap_response_handler_t handlerFunc) noexcept
{
  coap_register_response_handler(m_ctx, handlerFunc);
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
  if(!coap_add_option(pdu, COAP_OPTION_OBSERVE, len, buf)) {
    coap_show_pdu(COAP_LOG_WARN, pdu);
    std::string error = __FUNCTION__ + std::string("coap_add_option is fail! path is:") +  urlPath;
    throw std::runtime_error(error.c_str());
  }
  /*
    添加token，使用观察者机制必须要带上token。
    在CoAP协议中，观察者机制是通过在观察请求和响应之间传递一个令牌来实现的。
    这个令牌标识了观察的资源。
    当服务器端有关于该资源的状态更新时，它会发送一个包含相同令牌的观察响应给客户端，以此来通知客户端资源状态的变化。
  */ 
  uint8_t tokens[8];
  coap_binary_t token { 8, tokens };
  coap_session_new_token(m_session, &token.length, token.s);
  if(!coap_add_token(pdu, token.length, token.s)) {
    coap_show_pdu(COAP_LOG_WARN, pdu);
    std::string error = __FUNCTION__ + std::string("coap_add_token is fail! path is:") +  urlPath;
    throw std::runtime_error(error.c_str());
  }
  // 值得注意的是，当使用观察机制进行通信时，需要小心处理分块传输和大数据接收逻辑，并确保正确设置选项和使用正确的API以避免出现问题。
  // coap_context_set_block_mode()函数用于设置块传输模式，该函数的第二个参数为COAP_BLOCK_USE_LIBCOAP，表示使用libcoap内部的块传输处理逻辑。
  // coap_context_set_block_mode(m_ctx, COAP_BLOCK_USE_LIBCOAP);
  
  // 解释URL
  coap_uri_t url;
  auto urlPath_const_uint8_t = reinterpret_cast<const uint8_t*>(urlPath.c_str());
  if(coap_split_uri(urlPath_const_uint8_t, urlPath.length(), &url) < 0) {
    std::string error = __FUNCTION__ + std::string("coap_split_uri is fail! path is:") +  urlPath;
    throw std::runtime_error(error.c_str());
  }
  // 无需解析查询参数
  //auto queryCount = coap_split_query(url.query.s, url.query.length, queryBuff, nullptr);
  
  // 解析Path
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
  subscriptionkey key = std::make_pair(urlPath, token);
  m_subscriptionsCallbackFuncs.emplace(key, callback);
}

void SimpleClient::unsubscribe(const std::string &urlPath)
{
  auto iter = std::find_if(m_subscriptionsCallbackFuncs.begin(), m_subscriptionsCallbackFuncs.end(),
                            [urlPath](const auto& pair) { return pair.first.first == urlPath; });
  if (iter != m_subscriptionsCallbackFuncs.end()) {
      auto token = static_cast<coap_binary_t*>(const_cast<coap_binary_t*>(&iter->first.second));
      if(!coap_cancel_observe(m_session, token, COAP_MESSAGE_NON)) {
        std::string error = __FUNCTION__ + std::string("coap_cancel_observe is fail! path is:") +  urlPath;
        throw std::runtime_error(error.c_str());
      }else
        m_subscriptionsCallbackFuncs.erase(iter);
      
  }else {
      std::string error = __FUNCTION__ + std::string(" No find subscriptions! path is:") +  urlPath;
      throw std::runtime_error(error.c_str());
  }
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
  // 获取token
  coap_bin_const_t token = coap_pdu_get_token(received);
 
  coap_log_debug("** process incoming %d.%02d response:\n", Coap::getResponseStatus(rcv_code), Coap::getRequestMethod(rcv_code));
  coap_show_pdu(COAP_LOG_INFO, received);

  // 表示服务器返回了一个RST
  if (rcv_type == COAP_MESSAGE_RST) {
    coap_log_info("got RST\n");
    return COAP_RESPONSE_OK;
  }

  // 表示服务器返回了一个ACK
  if(auto iter = m_requestsCallbackFuncs.find(mid); 
    iter != m_requestsCallbackFuncs.end()) 
  {
    iter->second(sent, received);
    m_requestsCallbackFuncs.erase(iter);
    return COAP_RESPONSE_OK;
  }

  // 表示服务器推送了订阅的消息
  auto iter = std::find_if(m_subscriptionsCallbackFuncs.begin(), m_subscriptionsCallbackFuncs.end(),
                            [token, received](const auto& pair) {
                              std::string urlPath((const char*)coap_get_uri_path(received)->s, coap_get_uri_path(received)->length);
                              auto destUrlPath = pair.first.first;
                              auto destToken = pair.first.second;
                              return destUrlPath == urlPath && destToken.length == token.length && memcmp(destToken.s, token.s, token.length) == 0;
                            });
  if (iter != m_subscriptionsCallbackFuncs.end()) {
    iter->second(sent, received);
    return COAP_RESPONSE_OK;
  }

  if(!sent && (rcv_type == COAP_MESSAGE_CON || rcv_type == COAP_MESSAGE_NON)) {
    coap_log_info("got an unsolicited CON or NON, sending RST\n");
    //coap_send_rst(session, received);
    return COAP_RESPONSE_FAIL;
  }
  else
    return COAP_RESPONSE_OK;
}
