#include <coap3/coap.h>
#include "DefaultHandling.h"
#include "coap/DataStruct/BinaryConst.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"
#include "coap/Log.h"
#include <iostream>

namespace CoapPlusPlus {

SendersManager::DefaultHandling::DefaultHandling(BinaryConst token) noexcept
    : Handling(COAP_INVALID_MID, token) { }

bool SendersManager::DefaultHandling::onAck(Session &session, const RequestPdu *request, const ResponsePdu *response) noexcept
{
    Log::Logging(LOG_LEVEL::INFO, "The corresponding Handling is not found for onAck, use the DefaultHandling instead.\n");
    Pdu::LogPdu(LOG_LEVEL::INFO, request);
    Pdu::LogPdu(LOG_LEVEL::INFO, response);
    //std::flush(std::cout);
    return true;
}

void SendersManager::DefaultHandling::onNAck(Session &session, RequestPdu request, NAckReason reason) noexcept
{
    Log::Logging(LOG_LEVEL::INFO, "The corresponding Handling is not found for onNAck, use the DefaultHandling instead.\n");
    Pdu::LogPdu(LOG_LEVEL::INFO, &request);
    //std::flush(std::cout);
}

}; // namespace CoapPlusPlus