#include "SendersManagerHandlerWrapper.h"
#include "DefaultHandling.h"
#include "coap/exception.h"
#include "coap/DataStruct/BinaryConstView.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"
#include "coap/Session.h"
#include "coap/Handling.h"

namespace CoapPlusPlus
{

void SendersManager::SendersManagerHandlerWrapper::NackHandler(coap_session_t *session, const coap_pdu_t *sent, coap_nack_reason_t reason, const coap_mid_t mid)
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
    Handling* handling;
    bool isDefaultHandling = false;
    try { handling = s->getSendersManager().getHandling(token); }
    catch (TargetNotFoundException &e) { 
        handling = s->getSendersManager().m_defaultHandling; 
        isDefaultHandling = true;
        if (handling == nullptr) {
            std::string message = "internal error! default handling is nullptr and " + std::string(e.what());
            throw std::runtime_error(message.c_str());
        }
    }

    auto pdu = RequestPdu(const_cast<coap_pdu_t*>(sent), BinaryConst::DeepCopy(&coap_token));
    handling->onNAck(*s, std::move(pdu), static_cast<Handling::NAckReason>(reason));
    if (handling->isFinished() && isDefaultHandling == false) {
        s->getSendersManager().removeHandling(token);
    }
    
}catch(std::exception &e)
{
    coap_log_warn("NackHandler Error: %s\n", e.what());
}

coap_response_t SendersManager::SendersManagerHandlerWrapper::AckHandler(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, const coap_mid_t mid)
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
    // 获取handling
    Handling* handling;
    bool isDefaultHandling = false;
    try { handling = s->getSendersManager().getHandling(response_token); }
    catch (TargetNotFoundException &e) { 
        handling = s->getSendersManager().m_defaultHandling; 
        isDefaultHandling = true;
        if (handling == nullptr) {
            std::string message = "internal error! default handling is nullptr and " + std::string(e.what());
            throw std::runtime_error(message.c_str());
        }
    }

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
        s->getSendersManager().removeHandling(response_token);
    }
    return COAP_RESPONSE_OK;
}catch(std::exception &e)
{
    coap_log_warn("AckHandler Error: %s\n", e.what());
    return COAP_RESPONSE_FAIL;
}


} // namespace CoapPlusPlus