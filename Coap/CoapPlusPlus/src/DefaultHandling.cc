#include <coap3/coap.h>
#include "DefaultHandling.h"
#include "coap/DataStruct/BinaryConst.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"

namespace CoapPlusPlus {

SendersManager::DefaultHandling::DefaultHandling(BinaryConst token) noexcept
    : Handling(COAP_INVALID_MID, token) { }

bool SendersManager::DefaultHandling::onAck(Session &session, const RequestPdu *request, const ResponsePdu *response) noexcept
{
    Pdu::LogPdu(LOG_LEVEL::INFO, request);
    Pdu::LogPdu(LOG_LEVEL::INFO, response);
    return true;
}

void SendersManager::DefaultHandling::onNAck(Session &session, RequestPdu request, NAckReason reason) noexcept
{
    Pdu::LogPdu(LOG_LEVEL::INFO, &request);
}

}; // namespace CoapPlusPlus