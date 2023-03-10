#include <coap3/coap.h>
#include <map>
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

    void registerEvent(std::function<coap_event_handler_t> handlerFunc) noexcept;
    void registerNack(std::function<coap_nack_handler_t>  handlerFunc) noexcept;
    void registerResponse(std::function<coap_response_handler_t> handlerFunc) noexcept;

    void sendRequest(const coap_pdu_t* pdu, CallbackFunc callback);
    void observe(const std::string& urlPath, CallbackFunc callback);
    void unsubscribe(const std::string& urlPath, CallbackFunc callback);

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
    static std::map<std::string, CallbackFunc> m_subscriptionsCallbackFuncs;
};


// class CoapClient {
// public:
//     class Builder;

//     ~CoapClient() {
//         coap_session_release(session_);
//         coap_free_context(context_);
//     }

//     void send_request(coap_pdu_t* pdu,
//                       std::function<void(coap_pdu_t*)> callback) {
//         coap_tid_t tid = coap_send(session_, pdu);
//         if (tid == COAP_INVALID_TID) {
//             throw std::runtime_error("Failed to send CoAP PDU");
//         }
//         pending_requests_[tid] = callback;
//     }

//     void observe(const std::string& path,
//                  std::function<void(coap_pdu_t*)> callback) {
//         coap_subscription_t* sub =
//             coap_subscribe(session_, options_.data(), options_.size(),
//                            (const uint8_t*)path.c_str(), COAP_SUBSCRIPTION_ALWAYS, nullptr);
//         if (!sub) {
//             throw std::runtime_error("Failed to subscribe to resource");
//         }
//         subscriptions_[sub] = callback;
//     }

//     void run() {
//         while (coap_can_receive(context_)) {
//             coap_read(context_);
//         }
//     }

// private:
//     explicit CoapClient(const std::string& host, unsigned int port)
//         : host_(host), port_(port) {
//         init();
//         coap_register_response_handler(context_, session_,
//                                         [](coap_context_t* context,
//                                            coap_session_t* session,
//                                            coap_pdu_t* received,
//                                            const coap_tid_t id) {
//             auto it = pending_requests_.find(id);
//             if (it != pending_requests_.end()) {
//                 it->second(received);
//                 pending_requests_.erase(it);
//             }
//             else {
//                 auto sub_it =
//                     std::find_if(subscriptions_.begin(), subscriptions_.end(),
//                                     [=](const auto& pair) {
//                                         return pair.first->id == id;
//                                     });
//                 if (sub_it != subscriptions_.end()) {
//                     sub_it->second(received);
//                 }
//             }
//             coap_delete_pdu(received);
//         });
//     }

//     void init() {
//         coap_startup();
//         context_ = coap_new_context(nullptr);
//         if (!context_) {
//             throw std::runtime_error("Failed to create CoAP context");
//         }
//         coap_address_t dst;
//         coap_address_init(&dst);
//         int result =
//             coap_parse_uri((const uint8_t*)host_.c_str(), host_.length(),
//                             &dst); // 解析URI
//         if (result < 0) {
//             throw std::runtime_error("Failed to parse URI");
//         }
//         dst.port = port_;
//         session_ = coap_new_client_session(context_, nullptr, &dst, COAP_PROTO_UDP);
//         if (!session_) {
//             throw std::runtime_error("Failed to create client session");
//         }
//     }

//     std::string host_;
//     unsigned int port_ = COAP_DEFAULT_PORT;
//     std::vector<Option> options_;
//     std::map<coap_tid_t, std::function<void(coap_pdu_t*)>> pending_requests_;
//     coap_context_t* context_;
//     coap_session_t* session_;
//     std::map<coap_subscription_t*, std::function<void(coap_pdu_t*)>> subscriptions_;

//     friend class Builder;
// };
// 在这个版本中，我们使用std::map来跟踪所有未完成的请求和观察请求，并将它们与相应的回调函数关联起来。我们还添加了一个新的run()方法，它使客户端进入无限循环，以等待并处理所有事件（例如请求、响应、观察通知等）。

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