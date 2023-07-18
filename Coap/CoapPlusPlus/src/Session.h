/**
 * @file Session.h
 * @author Hulu
 * @brief coap_session_t 的C++封装
 * @version 0.3
 * @date 2023-07-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include <memory>
#include "coap/Information/GeneralInformation.h"

namespace CoapPlusPlus
{

class Context;
class SendersManager;
class Address;

class Session 
{
public:
    /**
     * @brief Construct a new Session object
     * 
     * @param raw_session
     * 
     * @exception invalid_argument raw_session 为空 
     */
    Session(coap_session_t* raw_session);
    ~Session();

    /**
     * @brief 获取会话协议类型。
     * 
     * @return 协议类型
     *      @retval Information::Protocol::Udp UDP协议
     *      @retval Information::Protocol::Tcp TCP协议
     *      @retval Information::Protocol::Tls TLS协议
     *      @retval Information::Protocol::Dtls DTLS协议
     *      @retval Information::Protocol::None 未知协议
     */
    Information::Protocol getProtocol() const noexcept;

    /**
     * @brief 获取当前会话的状态
     * 
     * @return 会话状态
     *      @retval Information::SessionState::NoneState 会话未建立
     *      @retval Information::SessionState::Connecting 正在连接中
     *      @retval Information::SessionState::Handshaking 会话正在握手
     *      @retval Information::SessionState::Csm 正在进行Csm交换
     *      @retval Information::SessionState::Established 会话已建立
     */
    Information::SessionState getSessionState() const noexcept;

    /**
     * @brief 从会话中获取本地IP地址和端口。
     * 
     * @return Address 保存了本地IP地址和端口的对象 @see Address
     * 
     * @exception InternalException 内部错误，无法获取本地地址
     */
    Address getLocalAddress() const;

    /**
     * @brief 从会话中获取远程IP地址和端口。
     * 
     * @return Address 保存了本地IP地址和端口的对象 @see Addresss
     * 
     * @exception InternalException 内部错误，无法获取本地地址
     * @note 对于客户端，当恢复组播地址时，在调用下一次coap_send()之前，这可能是组播请求的响应IP地址。
     */
    Address getRemoteAddress() const;

    /**
     * @brief 得到一个SendersManager对象。
     * 
     * @return SendersManager，SendersManager的生命周期由Session管理 @see SendersManager
     */
    SendersManager* getSendersManager() noexcept { return m_senderManager; } 

    /**
     * @brief 获取下一次重发前CoAP初始ACK响应超时。
     * @see RFC7252 ACK_TIMEOUT
     * 
     * @return 预期收到ACK或未收到CON报文的响应的秒数
     */
    float getAckTimeout() const noexcept;

    /**
     * @brief 设置下一次重发前CoAP初始ACK响应超时。
     * @see RFC7252 ACK_TIMEOUT
     * 
     * @param seconds 预期收到ACK或未收到CON报文的响应的秒数, 如果设置小于等于0，则会被设置为2.0
     * 
     * @note 默认值为2.0秒，通常不应该更改这个值。
     */
    void setAckTimeout(float seconds) noexcept;

    /**
     * @brief 获取请求报文发送停止前的最大重传次数
     * @see RFC7252 MAX_RETRANSMIT
     * 
     * @return 当前最大重传次数
     */
    uint16_t getMaxRetransmit() const noexcept;

    /**
     * @brief 设置请求报文发送停止前的最大重传次数。
     * @see RFC7252 MAX_RETRANSMIT
     * 
     * @param value 停止发送报文前的报文重传次数
     * 
     * @note 默认值为4次，通常不应该更改这个值。
     */
    void setMaxRetransmit(uint16_t value) noexcept;

public:
    /**
     * @brief 从Session得到一个Context对象的引用。
     * 
     * @param session 会话对象
     * @return Context上下文对象的引用
     * 
     * @exception std::invalid_argument 会话对象为空时抛出该异常
     * @exception TargetNotFoundException 会话对象对应的上下文对象不存在时抛出该异常
     */
    static const Context* GetContext(const Session* session);

private:
    const coap_session_t* getSession() const noexcept { return m_session; }
    void sessionInit() noexcept;

private:
    coap_session_t* m_session = nullptr;
    SendersManager* m_senderManager = nullptr;
};


} // namespace CoapPlusPlus