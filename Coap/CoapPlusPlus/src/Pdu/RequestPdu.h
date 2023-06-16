/**
 * @file RequestPdu.h
 * @author Hulu
 * @brief 请求PDU
 * @version 0.1
 * @date 2023-06-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "Pdu.h"
#include "Payload.h"
#include "coap/DataStruct/BinaryConstView.h"
#include <optional>

namespace CoapPlusPlus
{

class RequestPdu : public Pdu
{
public:
    RequestPdu(coap_pdu_t* rawPdu, BinaryConstView token);
    ~RequestPdu() = default;

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

    RequestCode code() const noexcept { return m_requestCode; }

    bool setCode(RequestCode code) noexcept;

    /**
     * @brief 获取当前PDU的token, 每个PDU都必须要有一个token，用来匹配响应PDU
     * 
     * @return token
     */
    BinaryConstView token() const noexcept { return m_token; }

private:
    RequestCode m_requestCode;
    std::optional<Payload> m_payload;
    BinaryConstView m_token;
};



};// namespace CoapPlusPlus