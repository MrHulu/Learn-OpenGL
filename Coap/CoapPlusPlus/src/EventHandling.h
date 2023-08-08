/**
 * @file EventHandling.h
 * @author Hulu
 * @brief libcoap事件处理类定义
 * @version 0.1
 * @date 2023-08-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

namespace CoapPlusPlus
{

/**
 * @brief 事件处理类定义
 * 
 */
class Session;
class EventHandling
{
    friend class Context;
public:
    EventHandling() noexcept {}
    ~EventHandling() noexcept {}

protected:
    /**
     * @brief (D)TLS 会话关闭时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onDtlsClosed(Session session) noexcept = 0;

    /**
     * @brief (D)TLS 会话连接时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onDtlsConnected(Session session) noexcept = 0;

    /**
     * @brief (D)TLS 会话重新协商时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onDtlsRenegotiation(Session session) noexcept = 0;
    
    /**
     * @brief (D)TLS 发生错误时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onDtlsError(Session session) noexcept = 0;

    /**
     * @brief Tcp 层被关闭时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onTcpClosed(Session session) noexcept = 0;

    /**
     * @brief Tcp 层连接时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onTcpConnected(Session session) noexcept = 0;

    /**
     * @brief TCP 层因某种原因失效时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onTcpFailed(Session session) noexcept = 0;

    /**
     * @brief 交换 CSM 信息后 TCP 层关闭时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onSessionClosed(Session session) noexcept = 0;

    /**
     * @brief TCP 层完成 CSM 信息交换时触发。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onSessionConnected(Session session) noexcept = 0;

    /**
     * @brief 当 TCP 层在交换 CSM 信息后失败时触发。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onSessionFailed(Session session) noexcept = 0;

    /**
     * @brief 当未收到全部大块内容时触发。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onPartialBlock(Session session) noexcept = 0;

    /**
     * @brief 当未传送完一个大体时触发。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onXmitBlockFail(Session session) noexcept = 0;

    /**
     * @brief 服务器发现了一个新的客户端会话时触发。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     * 
     * @note 该函数只在服务器端有效。
     * @attention 会话可能还不是一个完全建立的连接，也可能是指处于握手阶段的 DTLS 会话。
     */
    virtual void onServerSessionConnected(Session session) noexcept = 0;

    /**
     * @brief 服务器发现了一个客户端会话被关闭(例如客户端没有活跃并且超时或者超过了空闲会话的最大数量)时触发。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     * 
     * @note 该函数只在服务器端有效。
     * @attention 调用该函数时，会话内仍然包含有效数据
     */
    virtual void onServerSessionClosed(Session session) noexcept = 0;

    /**
     * @brief 当收到格式错误的数据包时触发。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onBadPacket(Session session) noexcept = 0;

    /**
     * @brief 当信息被重传时触发。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onMSGRetransmit(Session session) noexcept = 0;

    /**
     * @brief 当客户端请求握手，直到达到最大重传次数后服务器均无响应时自动触发该函数。
     * 
     * @param session CoAP上下文相关联的当前Session对象
     */
    virtual void onKeepAliveFailured(Session session) noexcept = 0;

};

}; // namespace CoapPlusPlus