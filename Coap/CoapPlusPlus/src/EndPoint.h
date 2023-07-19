/**
 * @file EndPoint.h
 * @author Hulu
 * @brief libcoap中的coap_endpoint_t的C++封装: EndPoint类
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>

namespace CoapPlusPlus
{

class EndPoint
{
    coap_endpoint_t* getEndPoint() const noexcept { return m_ep; }
    EndPoint& operator=(const EndPoint&) = delete;
    EndPoint& operator=(EndPoint&&) = delete;
    EndPoint(const EndPoint&) = delete;
    EndPoint(EndPoint&&) = delete;
public:
    /**
     * @brief 构造一个EndPoint对象
     * @details 类EndPoin是coap_endpoint_t的C++封装。
     *          EndPoint是CoAP网络中的一个通信接口，它可以被视为CoAP服务器或客户端的一个网络套接字。
     *          一个endpoint可以处理多个客户端与服务器之间的会话。它包含了一个网络地址和端口，以及关联的传输协议（如UDP、DTLS等）。
     * 
     * @param ep 
     */
    EndPoint(coap_endpoint_t *ep) noexcept : m_ep(ep) { }
    ~EndPoint() noexcept { coap_free_endpoint(m_ep); }
private:
    coap_endpoint_t *m_ep = nullptr;
};



};  // namespace CoapPlusPlus