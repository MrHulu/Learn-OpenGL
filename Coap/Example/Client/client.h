#include <coap3/coap.h>
#include <map>
#include <thread>
#include <mutex>

class SimpleClient {

public:
    SimpleClient();
    ~SimpleClient();

    // void start(int timeout_ms = COAP_IO_WAIT);
    // void stop();

 private:
    // std::mutex m_mutex;
    // bool m_flag = true;
    // int  m_timeout_ms = COAP_IO_WAIT;
    // std::thread* m_thread {};
    // void startCoapProcess();

private:
    coap_address_t      m_address;
    coap_context_t*     m_ctx {};
    coap_session_t*     m_session {};
    //std::map<std::string, coap_resource_t *> m_resources;
};