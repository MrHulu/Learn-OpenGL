#include "Session.h"
#include "coap/exception.h"
#include "coap/DataStruct/Address.h"
#include "coap/SendersManager.h"

namespace CoapPlusPlus
{

Session::Session(coap_session_t *raw_session) : m_session(raw_session)
{
    if(raw_session == nullptr)
        throw std::invalid_argument("Failed to construct a new Session object, session is nullptr");
    sessionInit();
}

Session::~Session()
{
    if(m_session) {
        coap_session_set_app_data(m_session, nullptr);
        coap_session_release(m_session);
    }
    delete m_senderManager;
}

Information::Protocol Session::getProtocol() const noexcept
{
    auto pro = coap_session_get_proto(m_session);
    return static_cast<Information::Protocol>(pro);
}

Information::SessionState Session::getSessionState() const noexcept
{
    auto state = coap_session_get_state(m_session);
    return static_cast<Information::SessionState>(state);
}

Address Session::getLocalAddress() const
{
    auto raw_addr = coap_session_get_addr_local(m_session);
    if(raw_addr == nullptr)
        throw InternalException("Failed to call the Session::getLocalAddress()");
    return Address(*raw_addr);
}

Address Session::getRemoteAddress() const
{
    auto raw_addr = coap_session_get_addr_remote(m_session);
    if(raw_addr == nullptr)
        throw InternalException("Failed to call the Session::getRemoteAddress()");
    return Address(*raw_addr);
}

float Session::getAckTimeout() const noexcept
{
    auto fixed = coap_session_get_ack_timeout(m_session);
    return fixed.integer_part + fixed.fractional_part / 1000.f;
}

void Session::setAckTimeout(float seconds) noexcept
{
    coap_fixed_point_t fixed;
    fixed.integer_part = static_cast<uint16_t>(seconds);
    fixed.fractional_part = static_cast<uint16_t>((seconds - fixed.integer_part) * 1000);
    coap_session_set_ack_timeout(m_session, fixed);
}

uint16_t Session::getMaxRetransmit() const noexcept
{
    return coap_session_get_max_retransmit(m_session);
}

void Session::setMaxRetransmit(uint16_t value) noexcept
{
    if(value == 0)
        return ;
    coap_session_set_max_retransmit(m_session, value);
}

uint16_t Session::getNSTART() const noexcept
{
    return coap_session_get_nstart(m_session);
}

void Session::setNSTART(uint16_t count) noexcept
{
    if(count == 0)
        return ;
    coap_session_set_nstart(m_session, count);
}

const Context *Session::GetContext(const Session *session)
{
    if(session == nullptr)
        throw std::invalid_argument("Failed to get the context, session is nullptr");
    auto coap_context = coap_session_get_context(session->getSession());
    if(coap_context == nullptr)
        throw TargetNotFoundException("Failed to get the context, unable to find context");
    auto context = static_cast<Context*>(coap_get_app_data(coap_context));
    if(context == nullptr)
        throw TargetNotFoundException("Failed to get the context, unable to find context");
    return context;
}

void Session::sessionInit() noexcept
{
    m_senderManager = new SendersManager(*m_session);
    coap_session_set_app_data(m_session, this);
}

} // namespace CoapPlusPlus