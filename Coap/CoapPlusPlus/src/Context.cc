#include "Context.h"
#include "coap/client/SimpleClient.h"
#include "coap/server/SimpleServer.h"
#include "coap/server/EndPoint.h"
#include "coap/exception.h"

#include <stdexcept>

namespace Coap {

Context::Context() {
    m_ctx = coap_new_context(nullptr);
    if (m_ctx == nullptr) {
        throw CallCoapLibFuncException("Failed to call the coap_new_context function!");
    }
    coap_startup();
}

Context::~Context() {
    m_endpoints.clear();
    if (m_ctx != nullptr) {
        coap_free_context(m_ctx);
        m_ctx = nullptr;
    }
    coap_cleanup();
}

std::shared_ptr<SimpleServer> Context::server() noexcept
{
    if (m_server == nullptr) {
        m_server = std::make_shared<SimpleServer>(*this);
    }
    return m_server;
}

std::shared_ptr<SimpleClient> Context::client() noexcept
{
    if (m_client == nullptr) {
        m_client = std::make_shared<SimpleClient>(*this);
    }
    return m_client;
}

void Context::addEndPoint(int port, int proto)
{
  auto it = m_endpoints.find(port);
  if (it != m_endpoints.end()) {
      throw std::invalid_argument("EndPoint already exists");
  }
  auto ep = createEndPoint(port, proto);
  auto endpoint = std::make_shared<EndPoint>(ep);
  m_endpoints.emplace(port, endpoint);
}

void Context::removeEndPoint(int port)
{
  auto it = m_endpoints.find(port);
  if (it == m_endpoints.end()) {
      throw std::invalid_argument("EndPoint not found");
  }
  m_endpoints.erase(it);
}

coap_endpoint_t *Context::createEndPoint(int port, int proto)
{
    coap_address_t  addr;
    coap_address_init(&addr);
    addr.addr.sin.sin_family = AF_INET;
    addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
    addr.addr.sin.sin_port = htons(port);
    auto ep = coap_new_endpoint(m_ctx, &addr, (coap_proto_t)proto);
    if (ep == nullptr) {
        throw CallCoapLibFuncException("Failed to call the coap_new_endpoint function!");
    }
    return ep;
}

// std::shared_ptr<Resource> Context::resource(const std::string& url) const {
//     auto it = m_resources.find(url);
//     if (it == m_resources.end()) {
//         throw std::out_of_range("Resource not found, url: " + url + "");
//     }
//     return std::shared_ptr<Resource>(it->second);
// }

// void Context::addResource(std::unique_ptr<Resource> resource) {
//     auto res = resource.get();
//     auto it = m_resources.find(res->getUriPath());
//     if (it != m_resources.end()) {
//         throw std::invalid_argument("Resource already exists, url: " + res->getUriPath() + "");
//     }
//     m_resources[res->getUriPath()] = res;
//     resource.release(); // ownership being taken by m_resources
// }

// void Context::deleteResource(const std::string &url)
// {
//     auto it = m_resources.find(url);
//     if (it != m_resources.end()) {
//         auto res = it->second;
//         delete it->second;
//         m_resources.erase(it);
//     }
// }

}; // namespace Coap