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
  coap_session_release(m_session);
  coap_free_context(m_ctx);
  coap_cleanup();
  //stop();
}

// void SimpleClient::start(int timeout_ms)
// {
//   stop();
//   std::lock_guard<std::mutex> lock(m_mutex);
//   m_flag = true;
//   m_timeout_ms = timeout_ms;
//   m_thread = new std::thread(&SimpleClient::startCoapProcess, this);
// }

// void SimpleClient::stop()
// {
//   {
//     std::lock_guard<std::mutex> lock(m_mutex);
//     m_flag = false;
//   }
//   if(m_thread) {
//     m_thread->join();
//     delete m_thread;
//     m_thread = nullptr;
//   }
// }

// void SimpleClient::startCoapProcess()
// {
//   auto wait_ms = m_timeout_ms;   
//   coap_context_t *ctx;
//   while (true) { 
//     bool flag; 
//     {
//       std::lock_guard<std::mutex> lock(m_mutex);
//       flag = m_flag;
//       ctx = m_ctx;
//     }
//     if (!flag) break;
//     auto result = coap_io_process(ctx, wait_ms);
//     if (result < 0)  
//       break; 
//     else if (result && result < wait_ms)
//       wait_ms -= result; 
//     else {
//       std::lock_guard<std::mutex> lock(m_mutex);
//       wait_ms = m_timeout_ms;
//     }
//   }
// }
