#include "SimpleServer.h"
#include "coap/Context.h"
#include "EndPoint.h"
#include "Resource.h"

#include <stdexcept>
#include <iostream>
#include <functional>

namespace Coap {

SimpleServer::SimpleServer(Context& ctx) : ctx(ctx) { }

SimpleServer::~SimpleServer()
{
  stop();
  for(auto iter = m_resources.begin(); iter != m_resources.end(); ++iter) {
    // 设置Resouece的coap_resource_t指针为nullptr，防止在Resouece析构函数中二次删除coap_resource_t
    iter->second->resetResource();
    m_resources.erase(iter);
  }
}

void SimpleServer::start(int timeout_ms)
{
  if(ctx.endPointCount() == 0)
    throw std::runtime_error("No EndPoint in Context, please call Context::addEndPoint()! ");
  stop();
  std::lock_guard<std::mutex> lock(m_mutex);
  m_flag = true;
  m_timeout_ms = timeout_ms;
  m_thread = new std::thread(&SimpleServer::startCoapProcess, this);
}

void SimpleServer::stop()
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

void SimpleServer::addTextResource(const std::string& url)
{
  auto resource = coap_resource_init(coap_make_str_const(url.c_str()), 0);
  auto func = [](coap_resource_t *resource,
              coap_session_t *session,
              const coap_pdu_t *request,
              const coap_string_t *query COAP_UNUSED,
              coap_pdu_t *response)
              {
                coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
                unsigned char buf[3];
                coap_add_option(response,
                      COAP_OPTION_CONTENT_FORMAT,
                      coap_encode_var_safe(buf, sizeof(buf), COAP_MEDIATYPE_TEXT_PLAIN),
                      buf);
                coap_add_data(response, 4, (const uint8_t *)"MOZA");
              };
  //coap_add_attr(resource, coap_make_str_const("ct"), coap_make_str_const("0"), 0); //不知道有什么用
  coap_register_request_handler(resource, COAP_REQUEST_GET, func);
  coap_add_resource(ctx.m_ctx, resource);
}

void SimpleServer::addResource(std::shared_ptr<Coap::Resource> resource)
{
  if(resource == nullptr) {
    throw std::invalid_argument("Resource is nullptr");
  }
  auto url = resource->getUriPath();
  auto iter = m_resources.find(url);
  if(iter != m_resources.end()) {
    throw std::invalid_argument("Resource already exists");
  }
  m_resources.emplace(url, resource);
  resource->setServer(this);
  coap_add_resource(ctx.m_ctx, resource->getResource());
}

void SimpleServer::startCoapProcess()
{
  uint32_t wait_ms = m_timeout_ms > 0 ? m_timeout_ms : 
      m_timeout_ms == 0 ? COAP_IO_WAIT : COAP_IO_NO_WAIT;
  coap_context_t *temp_ctx = ctx.m_ctx;
  while (true) { 
    bool flag; 
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      flag = m_flag;
    }
    if (!flag) break;
    auto result = coap_io_process(temp_ctx, wait_ms);
    if (result < 0)  
      break; 
    else if (result && (result < wait_ms))
      wait_ms -= result; 
    else {
      std::lock_guard<std::mutex> lock(m_mutex);
      wait_ms = m_timeout_ms > 0 ? m_timeout_ms : 
        m_timeout_ms == 0 ? COAP_IO_WAIT : COAP_IO_NO_WAIT;;
    }
  }
}

}; // namespace Coap