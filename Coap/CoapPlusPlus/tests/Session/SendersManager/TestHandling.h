#pragma once

#include <coap3/coap.h>
#include "coap/Handling.h"
#include "coap/DataStruct/BinaryConst.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"

namespace CoapPlusPlus
{

class TestHandlingData
{
    friend class TestHandling;
public:
    TestHandlingData(int number) noexcept : m_number(number) {  }
    bool operator==(const TestHandlingData& other) const noexcept { return m_number == other.m_number; }
    int number() const noexcept { return m_number; }
    bool isDestroy() const noexcept { return m_flag; }

private:
    int m_number = 0;
    bool m_flag = false;
};

class TestHandling : public Handling {

public:
    TestHandling(TestHandlingData* data, BinaryConst token) noexcept 
        : m_data(data)
        , Handling(COAP_INVALID_MID, token) {  }

    ~TestHandling() noexcept { 
        m_data->m_flag = true;
        coap_log_debug("~TestHandling(), %d\n", m_data->number()); 
    }

    bool onAck(Session& session, const RequestPdu* request, const ResponsePdu* response) noexcept override {
        m_data->m_number++;
        coap_log_info("TestHandling::onAck(): number[%d]\n", m_data->number());
        // Pdu::LogPdu(COAP_LOG_INFO, request);
        // Pdu::LogPdu(COAP_LOG_INFO, response);
        return true;
    }

    void onNAck(Session& session, RequestPdu request, NAckReason reason) noexcept override {
        m_data->m_number--;
        coap_log_warn("TestHandling::onNAck(): number[%d], reason[%d] ", m_data->number(), reason);
        Pdu::LogPdu(COAP_LOG_INFO, &request);
    }

    void readyDestroyed() noexcept override { 
        coap_log_info("TestHandling::readyDestroyed(), %d\n", m_data->number()); 
    }

    bool isFinished() noexcept override { return m_flag; }

    void setFinished(bool flag) noexcept { m_flag = flag;}

    TestHandlingData* data() noexcept { return m_data; }

private:
    bool m_flag = false;
    TestHandlingData* m_data = nullptr;
};


};