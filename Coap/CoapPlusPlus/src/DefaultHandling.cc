#include "DefaultHandling.h"
#include "coap/DataStruct/BinaryConst.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"

namespace CoapPlusPlus {

SendersManager::DefaultHandling::DefaultHandling(BinaryConst token) noexcept
    : Handling(COAP_INVALID_MID, token) { }

bool SendersManager::DefaultHandling::onAck(Session &session, const RequestPdu *request, const ResponsePdu *response) noexcept
{
    Pdu::LogPdu(COAP_LOG_INFO, request);
    Pdu::LogPdu(COAP_LOG_INFO, response);
    return true;
}

void SendersManager::DefaultHandling::onNAck(Session &session, RequestPdu request, NAckReason reason) noexcept
{

}

}; // namespace CoapPlusPlus