#include <coap3/coap.h>
#include <map>
#include <thread>
#include <mutex>

namespace Coap {
class Resource;
}; // namespace Coap

class SimpleServer {

public:
    SimpleServer();
    ~SimpleServer();

    /**
     * @brief 获取上下文对象
     * 
     * @return coap_context_t* 
     */
    coap_context_t* context() const { return m_ctx; }

    void start(int timeout_ms = COAP_IO_NO_WAIT);
    void stop();

    void addTextResource(const std::string& url);
    void addResource(std::shared_ptr<Coap::Resource> resource);

private:
    std::mutex m_mutex;
    bool m_flag = true;
    int  m_timeout_ms = COAP_IO_WAIT;
    std::thread* m_thread {};
    void startCoapProcess();

private:
    coap_address_t      m_address;
    coap_context_t*     m_ctx {};
    coap_endpoint_t*    m_endpoint {};
    std::map<std::string, std::shared_ptr<Coap::Resource>> m_resources;
    std::map<std::string, coap_resource_t*> m_rawResources;
};