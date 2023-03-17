#include <coap3/coap.h>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
//#include "SignalSlot.h"
class SimpleClient 
{
public:
    using CallbackFunc = std::function<void(const coap_pdu_t*, const coap_pdu_t*)>;
    SimpleClient();
    ~SimpleClient();

    /**
     * @brief 开始异步的Coap IO 处理
     *
     * @param timeout_ms 在做完任何处理后返回前等待新数据包的最小毫秒数。
     *                   如果为0，该调用将阻塞到下一个内部动作（如数据包重传）（如果有），或阻塞到收到下一个数据包（以较早者为准），并做必要的处理。
     *                   如果为-1，该函数将在处理后立即返回，而不等待任何新的输入数据包到达。
     */
    void asynchronousStartCoapProcess(int timeout_ms = COAP_IO_WAIT) noexcept;

    /**
     * @brief 开始同步的Coap IO 处理，注意该函数内部为while会阻塞当前线程
     *
     * @param timeout_ms 在做完任何处理后返回前等待新数据包的最小毫秒数。
     *                   如果为0，该调用将阻塞到下一个内部动作（如数据包重传）（如果有），或阻塞到收到下一个数据包（以较早者为准），并做必要的处理。
     *                   如果为-1，该函数将在处理后立即返回，而不等待任何新的输入数据包到达。
     */
    void synchronousStartCoapProcess(int timeout_ms = COAP_IO_WAIT) noexcept;

    /**
     * @brief 停止当前的Coap IO 处理
     */
    void stopCoapProcess() noexcept;

    void registerEvent(coap_event_handler_t handlerFunc) noexcept;
    void registerNack(coap_nack_handler_t  handlerFunc) noexcept;
    void registerResponse(coap_response_handler_t handlerFunc) noexcept;

    void sendRequest(coap_pdu_t* pdu, CallbackFunc callback);
    void observe(const std::string& urlPath, CallbackFunc callback);
    void unsubscribe(const std::string& urlPath);

    /* 默认的回调函数 */
public:
    void registerDefaultResponse() noexcept; 
    //void addDefaultRSTResponse(std::function<coap_response_t (const coap_pdu_t*, const coap_pdu_t*)> func) noexcept { m_rstReponseCallbacFunc = func; }
private:
    //std::function<coap_response_t (const coap_pdu_t*, const coap_pdu_t*)>  m_rstReponseCallbacFunc;
    static coap_response_t defaultResponse(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, coap_mid_t mid);

private:
    std::mutex m_mutex;
    bool m_flag = true;
    std::thread* m_thread {};

private:
    coap_address_t      m_address;
    coap_context_t*     m_ctx {};
    coap_session_t*     m_session {};
    static std::map<coap_mid_t, CallbackFunc> m_requestsCallbackFuncs;
    using subscriptionkey = std::pair<std::string, coap_binary_t>;
    struct SortByString {
        bool operator()(const subscriptionkey& lhs, const subscriptionkey& rhs) const {
            return lhs.first < rhs.first;
        }
    };
    static std::map<subscriptionkey, CallbackFunc, SortByString> m_subscriptionsCallbackFuncs;
};

// 使用示例：

// ```cpp
// int main() {
//     CoapClient client =
//         CoapClient::Builder()
//             .set_host("coap://example.com")
//             .set_port(5683)
//             .build();

// client.send_request(
//     coap_new_request(COAP_MESSAGE_CON, COAP_REQUEST_GET, coap_new_mid(),
//                      (const uint8_t*)"/hello-world", 12),
//     [](coap_pdu_t* response) {
//         std::cout << "Received response: "
//                   << std::string((char*)response->data, response->length)
//                   << std::endl;
//     });
// client.observe("/temperature", [](coap_pdu_t* response) {
//     std::cout << "Temperature: "
//               << std::string((char*)response->data, response->length)
//               << std::endl;
// });