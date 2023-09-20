#pragma once

#include <coap3/coap.h>
#include "coap/Handling.h"
#include "coap/DataStruct/BinaryConst.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"
#include "coap/Log.h"
#include <iostream>

namespace CoapPlusPlus
{

class HandlingExample : public Handling {

public:
    HandlingExample(BinaryConst token) noexcept
        : Handling(COAP_INVALID_MID, token) {  }

    ~HandlingExample() noexcept {  }

    bool onAck(Session& session, const RequestPdu* request, const ResponsePdu* response) noexcept override {
        Log::Logging(LOG_LEVEL::INFO, "onAck(Request) "); Pdu::LogPdu(LOG_LEVEL::INFO, request);
        Log::Logging(LOG_LEVEL::INFO, "onAck(Response) ");Pdu::LogPdu(LOG_LEVEL::INFO, response);
        //std::flush(std::cout);
        return true;
    }

    void onNAck(Session& session, RequestPdu request, NAckReason reason) noexcept override 
    {
        Log::Logging(LOG_LEVEL::INFO, "onNAck: %s\n", NAckReasonToString(reason));
        Pdu::LogPdu(LOG_LEVEL::INFO, &request);
        //std::flush(std::cout);
    }

    void readyDestroyed() noexcept override { }

    // 调用onNack或者onAck后，判断是否可以销毁
    bool isFinished() noexcept override { return true; }


private:
};


};