#pragma once

#include <coap3/coap.h>
#include "coap/ResourceInterface.h"
#include "coap/Session.h"
#include "coap/DataStruct/Address.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"
#include "coap/Pdu/Options.h"
#include "coap/Pdu/Option.h"
#include "coap/Pdu/OptFilter.h"
#include "coap/Pdu/Payload.h"
#include "coap/Pdu/Decoder.h"
#include "coap/DataStruct/BinaryConstView.h"

namespace CoapPlusPlus
{

class ResourceInterfaceExample : public ResourceInterface 
{
public:
    ResourceInterfaceExample(std::string text, Information::RequestCode code) noexcept 
     : m_text(text), ResourceInterface(code) {  }
    ~ResourceInterfaceExample() noexcept { }

    void onRequest(Session session, std::string query, ResponsePdu response, RequestPdu request) override
    {
        auto code = request.code();
        switch (code)
        {
        case Information::RequestCode::Get: {
            response.setCode(Information::ResponseCode::Content);
            Options options;
            options.insertContentFormatOption(Information::TextPlain);
            response.addOptions(options);
            auto content = std::vector<uint8_t>(m_text.begin(), m_text.end());
            auto payload = Payload(content.size(), content.data(), Information::TextPlain);
            response.setPayload(std::move(payload));
            break;
        }   
        case Information::RequestCode::Put: {
            auto payload = request.payload();
            if(request.isContainOption(Information::ContentFormat) == false 
                    || payload.size() == 0
                    || payload.type() != Information::TextPlain) {
                response.setCode(Information::ResponseCode::BadRequest);
                break;
            }
            response.setCode(Information::ResponseCode::Changed);
            auto vec = payload.data();
            m_text = std::string(vec.begin(), vec.end());
            break;
        }
        case Information::RequestCode::Post:
        case Information::RequestCode::Delete:
            response.setCode(Information::ResponseCode::MethodNotAllowed);
            break;
        }
        auto testlog = [](Pdu* pdu) {
            Log::Logging(INFO, "Mid[%d] Type[%s]\n", pdu->messageId(), PduInformation::MessageTypeToString(pdu->messageType()));
            Log::Logging(INFO, "Option[ ");
            for(auto opt: pdu->getOptions())
                std::cout <<" " <<  OptionNumberToString(opt.getNumber()) << " ";
            std::cout <<" ]" << std::endl; 
            auto vec = pdu->payload().data();
            Log::Logging(INFO, "Payload[ %s ] size[%d]\n", std::string(vec.begin(), vec.end()).c_str(), vec.size());
            Log::Logging(INFO, "*********************\n");
        };
        Log::Logging(INFO, "\n******** request  ******\n");
        Log::Logging(INFO, "Token[%s]\n", request.token().toHexString().c_str());
        Log::Logging(INFO, "Code[%s]\n", PduInformation::RequestCodeToString(request.code()));
        testlog(&request);

        Log::Logging(INFO, "\n******** response  ******\n");
        Log::Logging(INFO, "Code[%d]\n", PduInformation::getResponseStatusCode(response.code()));
        testlog(&response);
        
        Log::Logging(INFO, "LocalPort:%d, RemotePort:%d\n", session.getLocalAddress().getPort(), session.getRemoteAddress().getPort());
        Log::Logging(INFO, "Request:"); Pdu::LogPdu(INFO, &request);
        Log::Logging(INFO, "Response:"); Pdu::LogPdu(INFO, &response);
        std::flush(std::cout);
    }
private: 
    std::string m_text;
};


};