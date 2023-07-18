/**
 * @file Handling.h
 * @author Hulu
 * @brief 客户端每个Pdu请求的处理器，用于处理Pdu的响应，未响应，超时等情况。
 * @version 0.1
 * @date 2023-07-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

namespace CoapPlusPlus
{

class BinaryConstView;
class BinaryConst;
class Session;
class RequestPdu;
class ResponsePdu;

/**
 * @brief 客户端每个Pdu请求的处理器，用于处理Pdu的响应，未响应，超时等情况。
 * 
 */
class Handling
{
public:
    enum NAckReason {
        TooManyRetransmit = 0,  // 消息已被重传多次，但未收到确认
        NotDelivered,           // 表示报文无法发送，通常是因为无法到达目的地或者报文格式或内容有错误。
        Reset,                  // 从接收方收到一条重置消息（RST）作为对消息的响应。这是一个收件人不理解报文或者无法处理报文的信号
        TlsFailed,              // 这表示TLS（传输层安全）级别的失败，例如握手失败，证书无效，或者密码套件不匹配。
        IcmpIssue,              // 表示收到了一条ICMP（Internet控制报文协议）错误报文，例如 "目标不可达 "或 "超过时间"。
        BadResponse,            // 表示收到了一个意外的或无效的响应，例如一个畸形的数据包或一个代码不正确的响应。
        TlsLayerFailure,        // 表示传输层安全失败
        WsLayerFailure,         // 表示 WebSocket 层出现故障。
        WsFailure,              // 未能建立连接或连接意外关闭。
    };

    Handling(int mid, BinaryConst token) noexcept;
    virtual ~Handling() noexcept;

    /**
     * @brief 当收到对应的Pdu请求的响应时会自动调用该函数。
     * 
     * @param session 当前客户端会话
     * @param request 请求的Pdu, 有可能为空
     * @param response 响应的Pdu, 有可能为空
     * 
     * @retval true 该回应已经被处理
     * @retval false 该回应无法处理, 会给服务器发送RST消息
     */
    virtual bool onAck(Session& session, const RequestPdu* request, const ResponsePdu* response) noexcept = 0 ;

    /**
     * @brief 当发出Pdu请求在规定时间内没正常应答会自动调用该函数。
     * 
     * @param session 当前客户端会话
     * @param request 请求的Pdu
     * @param reason 未正常应答的原因
     */
    virtual void onNAck(Session& session, RequestPdu request, NAckReason reason) noexcept = 0;

    /**
     * @brief 当被销毁前会自动调用该函数。继承该类的子类需要在该函数中做好相关销毁工作。
     *        当前函数可能会在onAck或者onNAck之后调用。
     * 
     */
    virtual void readyDestroyed() noexcept = 0;

    /**
     * @brief 自动调用onAck或者onNAck之后，会自动调用该函数，用于判断当前处理器是否已经完成了自己的工作。
     * 
     * @return true 该处理器已经完成了自己的工作，可以被销毁
     * @return false 该处理器还没有完成自己的工作，不能被销毁
     * 
     * @note 如果该处理器是一个观察者处理器，建议继承该类的子类在该函数返回false，直到观察者被取消观察才会返回true。
     */
    virtual bool isFinished() noexcept = 0;

public:
    /**
     * @brief 获取对应的Pdu请求的MID
     * 
     * @return MID 
     */
    int messageId() const noexcept { return m_mid; }
    
    /**
     * @brief 获取对应的Pdu请求的Token
     * 
     * @return token @see BinaryConst 
     */
    BinaryConstView token() const noexcept;

private:
    int         m_mid;
    BinaryConst* m_token = nullptr;

};

} // namespace CoapPlusPlus