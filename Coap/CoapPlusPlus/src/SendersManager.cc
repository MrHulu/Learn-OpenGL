#include "SendersManager.h"
#include "DefaultHandling.h"
#include "coap/exception.h"
#include "coap/DataStruct/BinaryConstView.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"
#include "coap/Session.h"
#include "coap/Handling.h"
namespace CoapPlusPlus
{

SendersManager::SendersManager(coap_session_t &coap_session)
    : m_coap_session(&coap_session)
{
    registerHandlerInit();
}

SendersManager::~SendersManager()
{
    coap_log_debug("SendersManager::~SendersManager()\n");
    for (auto iter = m_handlings.begin(); iter != m_handlings.end(); ++iter) {
        iter->second->readyDestroyed();
        delete iter->second;
    }
    if (m_defaultHandling) {
        m_defaultHandling->readyDestroyed();
        delete m_defaultHandling;
    }
}

bool SendersManager::send(RequestPdu pdu, std::unique_ptr<Handling> handling)
{
    auto coap_pdu = pdu.getPdu();
    std::shared_ptr<BinaryConst> token = nullptr;
    try{
        if (coap_pdu == nullptr || m_coap_session == nullptr) {
            throw InternalException("pdu or session is nullptr");
        }
        token = std::make_shared<BinaryConst>(std::move(pdu.token().toBinaryConst()));
    } catch (std::exception& e) {
        if (handling.get() != nullptr) {
            handling->readyDestroyed();
            handling.reset();
        }
        coap_log_warn("send: %s\n", e.what());
        return false;
    }   
    
    if (handling.get() != nullptr) {
        auto iter = m_handlings.find(*token);
        if (iter != m_handlings.end()) {
            std::string error = std::string("The Handling for this token(")
                                + std::string(token->data().begin(), token->data().end()) 
                                + std::string(") already exists");
            if (handling.get() != nullptr) {
                handling->readyDestroyed();
                handling.reset();
            }
            //coap_log_warn("send: %s\n", error.c_str());
            throw AlreadyExistException(error.c_str());
        }
        m_handlings.insert(std::make_pair(*token, handling.release()));
    }
    auto mid = coap_send(m_coap_session, coap_pdu);
    return mid != COAP_INVALID_MID;
}


void SendersManager::updateDefaultHandling(std::unique_ptr<Handling> handling) noexcept
{
    if (handling.get() == nullptr)
        return;
    if (m_defaultHandling) {
        m_defaultHandling->readyDestroyed();
        delete m_defaultHandling;
    }
    m_defaultHandling = handling.release();
}

BinaryConst SendersManager::createToken() const
try
{
    if (m_coap_session == nullptr)
        throw InternalException("");
    size_t size;
    uint8_t tokenData[8];
    coap_session_new_token(m_coap_session, &size, tokenData);
    return BinaryConst::Create(size, tokenData);
}catch (std::exception &e)
{
    throw InternalException(e.what());
}

RequestPdu SendersManager::createRequest(Information::MessageType type, Information::RequestCode code) const
try
{
    if (&m_coap_session == nullptr)
        throw InternalException("");
    auto coap_pdu= coap_new_pdu(static_cast<coap_pdu_type_t>(type), 
                                static_cast<coap_pdu_code_t>(code), 
                                m_coap_session);
    return RequestPdu(coap_pdu, createToken());
}
catch (std::exception &e)
{
    throw InternalException(e.what());
}

std::shared_ptr<Handling> SendersManager::getHandling(const BinaryConstView &token) const
{
    auto t = token.toBinaryConst();
    auto iter = m_handlings.find(t);
    if (iter != m_handlings.end()) {
        Handling *handler = iter->second;
        return std::shared_ptr<Handling>(handler);
    }
    else
        throw TargetNotFoundException("Not found handling");
}

void SendersManager::registerHandlerInit() noexcept
{
    auto coap_context = coap_session_get_context(m_coap_session);
    coap_register_response_handler(coap_context, SendersManager::AckHandler); 
    coap_register_nack_handler(coap_context, SendersManager::NackHandler);
    try {
        m_defaultHandling = new DefaultHandling(createToken());
    }catch(...) {
        coap_log_warn("registerHandlerInit: internal error!\n");
    }
}

bool SendersManager::removeHandling(const BinaryConstView &token) noexcept
{
    auto t = token.toBinaryConst();
    auto iter = m_handlings.find(t);
    if (iter != m_handlings.end()) {
        iter->second->readyDestroyed();
        m_handlings.erase(iter);
        delete iter->second;
        return true;
    } else {
        return false;
    }
}

void SendersManager::NackHandler(coap_session_t *session, const coap_pdu_t *sent, coap_nack_reason_t reason, const coap_mid_t mid)
try{
    if (sent == nullptr || session == nullptr) {
        std::string message = "sent or session is nullptr, mid(" + std::to_string(mid) + "), reason(" + std::to_string(reason) + ")";
        throw std::runtime_error(message.c_str());
    }
    auto s = static_cast<Session*>(coap_session_get_app_data(session));
    if (s == nullptr)
        throw std::runtime_error("internal error! session doesn't save data to the app.");
    auto coap_token = coap_pdu_get_token(sent);
    auto token = BinaryConstView(&coap_token);
    
    // 获取handling
    std::shared_ptr<Handling> handling;
    bool isDefaultHandling = false;
    try { handling = s->getSendersManager()->getHandling(token); }
    catch (TargetNotFoundException &e) { 
        handling = std::shared_ptr<Handling>(s->getSendersManager()->m_defaultHandling); 
        isDefaultHandling = true;
        if (handling.get() == nullptr) {
            std::string message = "internal error! default handling is nullptr and " + std::string(e.what());
            throw std::runtime_error(message.c_str());
        }
    }

    auto pdu = RequestPdu(const_cast<coap_pdu_t*>(sent), BinaryConst::DeepCopy(&coap_token));
    handling->onNAck(*s, std::move(pdu), static_cast<Handling::NAckReason>(reason));
    if (handling->isFinished() && isDefaultHandling == false) {
        s->getSendersManager()->removeHandling(token);
    }
    
}catch(std::exception &e)
{
    coap_log_warn("NackHandler: %s\n", e.what());
}

coap_response_t SendersManager::AckHandler(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, const coap_mid_t mid)
try{
    if (received == nullptr || session == nullptr) {
        std::string message = "received or session is nullptr, mid(" + std::to_string(mid) + ")";
        throw std::runtime_error(message.c_str());
    }
    auto s = static_cast<Session*>(coap_session_get_app_data(session));
    if (s == nullptr)
        throw std::runtime_error("internal error! session doesn't save data to the app.");
    auto response = ResponsePdu(const_cast<coap_pdu_t*>(received));
    auto coap_response_token = coap_pdu_get_token(received);
    auto response_token = BinaryConstView(&coap_response_token);
    auto handling = s->getSendersManager()->getHandling(response_token);

    // 如果sent == nullptr，说明是一个非confirmable的请求，那么就不需要查找对应的request
    // 如果sent == nullptr, 说明是一个观察推送的消息
    if (sent == nullptr) {
        handling->onAck(*s, nullptr, &response);
    }
    else {
        auto coap_request_token = coap_pdu_get_token(sent);
        auto request_token = BinaryConstView(&coap_request_token);
        // 如果请求的token和响应的token不一致，那么就是一个错误的响应
        if (response_token != request_token) {
            std::string message = "Inconsistency between response token(" 
                                + std::string(response_token.data().begin(), response_token.data().end()) 
                                + ") and request token(" 
                                + std::string(request_token.data().begin(), response_token.data().end()) 
                                + ")";
            throw std::runtime_error(message.c_str());
        }
        auto request = RequestPdu(const_cast<coap_pdu_t*>(sent), BinaryConst::DeepCopy(&coap_request_token));
        
        handling->onAck(*s, &request, &response);
    }
    // 完成处理
    if (handling->isFinished()) {
        s->getSendersManager()->removeHandling(response_token);
    }
    return COAP_RESPONSE_OK;
}catch(std::exception &e)
{
    coap_log_warn("AckHandler: %s\n", e.what());
    return COAP_RESPONSE_FAIL;
}

} // namespace CoapPlusPlus