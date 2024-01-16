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
        if(response) {
            auto payload = response->payload();
            // if(response->isContainOption(Information::ContentFormat) == false) {
            //     Log::Logging(LOG_LEVEL::INFO, "response没有包含ContentFormat选项\n");     
            //     return false;
            // }
            // if(payload.size() == 0) {
            //     Log::Logging(LOG_LEVEL::INFO, "response没有包含payload\n");
            //     return false;
            // }
            // if(payload.type() != Information::Cbor) {
            //     Log::Logging(LOG_LEVEL::INFO, "response的payload不是cbor格式\n");
            //     return false;
            // }
            auto span = payload.data();
            // std::vector<char> byteArray(reinterpret_cast<const char*>(span.data()), reinterpret_cast<const char*>(span.data() + span.size()));
            // Log::Logging(LOG_LEVEL::INFO, "payload[%d]: %hex\n", payload.size(), payload.data().data());   
            for(uint8_t i = 0; i < span.size(); i++) {
                printf("%02x", span.data()[i]);
            }
            Log::Logging(LOG_LEVEL::INFO, "\n");
        }
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