#include "server.h"
#include "Resource.h"

#include <stdexcept>
#include <iostream>
#include <functional>

SimpleServer::SimpleServer()
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
  m_endpoint = coap_new_endpoint(m_ctx, &m_address, COAP_PROTO_UDP);
  if (!m_endpoint) {
      coap_free_context(m_ctx);
      coap_cleanup();
      throw std::runtime_error("coap_new_endpoint fail");
  }
}

SimpleServer::~SimpleServer()
{
  coap_free_context(m_ctx);
  coap_cleanup();
  stop();

}

void SimpleServer::start(int timeout_ms)
{
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
  coap_add_resource(m_ctx, resource);
  m_rawResources.insert({url, resource});
}

void SimpleServer::addResource(std::shared_ptr<Coap::Resource> resource)
{
  coap_add_resource(m_ctx, resource->getResource());
  m_resources.insert({resource->getUriPath(), resource});
}

void SimpleServer::startCoapProcess()
{
  auto wait_ms = m_timeout_ms;   
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
      wait_ms = m_timeout_ms;
    }
  }
}
