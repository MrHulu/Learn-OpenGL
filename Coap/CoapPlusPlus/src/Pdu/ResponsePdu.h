/**
 * @file ResponsePdu.h
 * @author Hulu
 * @brief 响应PDU
 * @version 0.2
 * @date 2023-07-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "Pdu.h"
#include "payload.h"
#include <optional>

namespace CoapPlusPlus {

class BinaryConstView;
class ResponsePdu : public Pdu {
public: 
    /**
     * @brief 构造一个响应PDU对象
     * 
     * @param pdu 传入非空的coap_pdu_t指针
     * @exception std::invalid_argument 当传入的pdu为空时抛出
     */
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

    /**
     * @brief 获取当前PDU的token, 每个PDU都必须要有一个token，用来匹配响应PDU
     * 
     * @return token
     */
    BinaryConstView requestToken() const noexcept;

private: 
    void init() noexcept;

private:
    ResponseCode m_responseCode;
    std::optional<Payload> m_payload;
};


}; // namespace CoapPlusPlus