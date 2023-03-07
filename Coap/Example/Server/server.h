#include <coap3/coap.h>
#include <map>
#include <thread>
#include <mutex>

class SimpleServer {

public:
    SimpleServer();
    ~SimpleServer();

    void start(int timeout_ms = COAP_IO_WAIT);
    void stop();

    // TODO: 临时做法，以后使用多态实现resource的管理、添加等
    void addTextResource(std::string url);

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
    std::map<std::string, coap_resource_t *> m_resources;
};