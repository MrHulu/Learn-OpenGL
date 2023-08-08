#include "EventHandling.h"
#include "Context.h"
#include "coap/Session.h"
#include <coap3/coap.h>
#include <stdexcept>

namespace CoapPlusPlus {

// const char *EventHandling::toString(EventType type) noexcept
// {
//     switch(type) {
//     case DTLS_CLOSED:
//         return "";
//     case DTLS_CONNECTED:
//         return "";
//     case DTLS_ERROR:
//         return "";
//     case DTLS_RENEGOTIATION:
//         return "";
//     case TCP_CLOSED:
//         return "";
//     case TCP_CONNECTED:
//         return "";
//     case TCP_FAILED:
//         return "";
//     case TCP_SESSION_CLOSED:
//         return "";
//     case TCP_SESSION_CONNECTED:
//         return "";
//     case TCP_SESSION_FAILED:
//         return "";
//     case PARTIAL_BLOCK:
//         return "";
//     case XMIT_BLOCK_FAILED:
//         return "";
//     case NEW_CLIENT:
//         return "";
//     case DEL_CLIENT:
//         return "";
//     case BAD_PACKET:
//         return "";
//     case MSG_RESEND_FAILED:
//         return "";
//     case KEEPALIVE_FAILED:
//         return "";
//     }
// }

void EventHandling::onEvent(EventType type, coap_session_t *session) noexcept
try{
    if(session == nullptr)
        throw std::runtime_error("session is nullptr");
    auto ctx = coap_session_get_context(session);
    if(ctx == nullptr)
        throw std::runtime_error("coap context is nullptr");
    auto context = static_cast<Context*>(coap_get_app_data(ctx));
    if(context == nullptr)
        throw std::runtime_error("context is nullptr");
    auto handling = context->m_eventHandling;
    if(handling) {
        switch(type) {
        case DTLS_CLOSED:
            handling->onDtlsClosed(Session(session, false));
            break;
        case DTLS_CONNECTED:
            handling->onDtlsConnected(Session(session, false));
            break;
        case DTLS_ERROR:
            handling->onDtlsError(Session(session, false));
            break;
        case DTLS_RENEGOTIATION:
            handling->onDtlsRenegotiation(Session(session, false));
            break;
        case TCP_CLOSED:
            handling->onTcpClosed(Session(session, false));
            break;
        case TCP_CONNECTED:
            handling->onTcpConnected(Session(session, false));
            break;
        case TCP_FAILED:
            handling->onTcpFailed(Session(session, false));
            break;
        case TCP_SESSION_CLOSED:
            handling->onSessionClosed(Session(session, false));
            break;
        case TCP_SESSION_CONNECTED:
            handling->onSessionConnected(Session(session, false));
            break;
        case TCP_SESSION_FAILED:
            handling->onSessionFailed(Session(session, false));
            break;
        case PARTIAL_BLOCK:
            handling->onPartialBlock(Session(session, false));
            break;
        case XMIT_BLOCK_FAILED:
            handling->onXmitBlockFail(Session(session, false));
            break;
        case NEW_CLIENT:
            handling->onServerSessionConnected(Session(session, false));
            break;
        case DEL_CLIENT:
            handling->onServerSessionClosed(Session(session, false));
            break;
        case BAD_PACKET:
            handling->onBadPacket(Session(session, false));
            break;
        case MSG_RESEND_FAILED:
            handling->onMSGRetransmit(Session(session, false));
            break;
        case KEEPALIVE_FAILED:
            handling->onKeepAliveFailured(Session(session, false));
            break;
        default:
            throw std::runtime_error("unknown event type");
        }
    }
}catch(std::exception& e) {
    coap_log(LOG_ERR, "onEvent() exception: %s\n", e.what());
}

} // namespace CoapPlusPlus