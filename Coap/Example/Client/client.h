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
    
    /**
     * @brief Construct a new Simple Client object
     * 
     * @exception std::runtime_error 无法创建coap上下文或者会话
     */
    SimpleClient();
    ~SimpleClient();

    /**
     * @brief Get the Session object
     * 
     * @return coap_session_t* 
     */
    coap_session_t* getSession() const noexcept { return m_session; }

    /**
     * @brief 开始异步的Coap IO 处理
     *
     * @param timeout_ms 在做完任何处理后返回前等待新数据包的最小毫秒数。
     *                   如果为0，该调用将阻塞到下一个内部动作（如数据包重传）（如果有），或阻塞到收到下一个数据包（以较早者为准），并做必要的处理。
     *                   如果为-1，该函数将在处理后立即返回，而不等待任何新的输入数据包到达。
     */
    void asynchronousStartCoapProcess(int timeout_ms = COAP_IO_NO_WAIT) noexcept;

    /**
     * @brief 开始同步的Coap IO 处理，注意该函数内部为while会阻塞当前线程
     *
     * @param timeout_ms 在做完任何处理后返回前等待新数据包的最小毫秒数。
     *                   如果为0，该调用将阻塞到下一个内部动作（如数据包重传）（如果有），或阻塞到收到下一个数据包（以较早者为准），并做必要的处理。
     *                   如果为-1，该函数将在处理后立即返回，而不等待任何新的输入数据包到达。
     */
    void synchronousStartCoapProcess(int timeout_ms = COAP_IO_NO_WAIT) noexcept;

    /**
     * @brief 停止当前的Coap IO 处理
     */
    void stopCoapProcess() noexcept;

    void registerEvent(coap_event_handler_t handlerFunc) noexcept;
    void registerNack(coap_nack_handler_t  handlerFunc) noexcept;
    void registerResponse(coap_response_handler_t handlerFunc) noexcept;

    /**
     * @brief 发送请求
     * 
     * @param pdu coap_pdu_t*
     * @param CallbackFunc std::function<void(const coap_pdu_t*, const coap_pdu_t*)> 该请求的回调函数
     * 
     * @exception std::runtime_error 无法发送数据包
     * @note 该函数会将pdu的mid值作为key，callback作为value，存入map中，当收到数据包时，会根据mid值找到对应的callback，并调用。
     *      如果注册了默认的回调函数，默认的回调函数会自动回复服务器，所以该回调函数不需要有返回值。
     */
    void sendRequest(coap_pdu_t* pdu, CallbackFunc callback);

    /**
     * @brief 订阅资源
     * 
     * @param urlPath 资源路径
     * @param CallbackFunc std::function<void(const coap_pdu_t*, const coap_pdu_t*)> 当收到一个订阅的资源的数据包时的回调函数
     */
    void observe(const std::string& urlPath, CallbackFunc callback);

    /**
     * @brief 取消订阅资源
     * 
     * @param urlPath 取消订阅的资源路径
     * 
     * @exception std::runtime_error ...TODO: 待补充
     * 
     */
    void unsubscribe(const std::string& urlPath);

    /**
     * @brief 注册默认的回调函数，默认的回调函数会打印出收到的数据包，
     *       会根据收到的数据包的mid值，从sendRequest(coap_pdu_t* pdu, CallbackFunc callback)中的回调函数中找到对应的回调函数，并调用。
     *       该函数也会根据收到的数据包的mid值，从observe(const std::string& urlPath, CallbackFunc callback)中的回调函数中找到对应的回调函数，并调用。
     */
    void registerDefaultResponse() noexcept; 
private:
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