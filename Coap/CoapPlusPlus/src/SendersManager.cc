#include "SendersManager.h"
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

SendersManager::SendersManager(coap_session_t &coap_session)
    : m_coap_session(&coap_session)
{
    registerHandlerInit();
}

SendersManager::~SendersManager()
{
    for (auto iter = m_handlings.begin(); iter != m_handlings.end(); ++iter) {
        iter->second->readyDestroyed();
        delete iter->second;
    }
    m_handlings.clear();
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
        if (handling->token() != pdu.token()) {
            handling->readyDestroyed();
            handling.reset();
            //coap_log_warn("send: handling token is not equal to pdu token\n");
            throw std::invalid_argument("handling token is not equal to pdu token");
        }
        auto iter = m_handlings.find(*token);
        if (iter != m_handlings.end()) {
            std::string error = std::string("The Handling for this token(")
                                + std::string(token->data().begin(), token->data().end()) 
                                + std::string(") already exists");
            handling->readyDestroyed();
            handling.reset();
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

Handling* SendersManager::getHandling(const BinaryConstView &token) const
{
    auto t = token.toBinaryConst();
    auto iter = m_handlings.find(t);
    if (iter != m_handlings.end()) {
        return iter->second;
    }
    else
        throw TargetNotFoundException("Not found handling");
}

void SendersManager::registerHandlerInit() noexcept
{
    auto coap_context = coap_session_get_context(m_coap_session);
    coap_register_response_handler(coap_context, SendersManager::SendersManagerHandlerWrapper::AckHandler); 
    coap_register_nack_handler(coap_context, SendersManager::SendersManagerHandlerWrapper::NackHandler);
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
        delete iter->second;
        m_handlings.erase(iter);
        return true;
    } else {
        return false;
    }
}

} // namespace CoapPlusPlus