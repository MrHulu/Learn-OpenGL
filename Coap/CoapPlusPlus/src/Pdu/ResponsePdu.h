/**
 * @file ResponsePdu.h
 * @author Hulu
 * @brief 响应PDU
 * @version 0.1
 * @date 2023-06-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "Pdu.h"
#include "payload.h"
#include <optional>

namespace CoapPlusPlus {

class ResponsePdu : public Pdu {
public:
    ResponsePdu(coap_pdu_t* pdu);
    ~ResponsePdu();

    Payload payload() const noexcept override;

    /**
     * @brief 给当前PDU设置payload
     * 
     * @param payload 
     * @return 设置成功返回true，否则返回false
     * 
     * @note 当函数设置成功后，再次调用将无法再次给pdu设置payload只会返回false
     */
    bool setPayload(Payload payload) noexcept override;

    ResponseCode code() const noexcept { return m_responseCode; }

    bool setCode(ResponseCode code) noexcept;

private:
    ResponseCode m_responseCode;
    std::optional<Payload> m_payload;
};


}; // namespace CoapPlusPlus