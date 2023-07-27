#include "Handling.h"
#include "coap/DataStruct/BinaryConstView.h"

namespace CoapPlusPlus
{
BinaryConstView Handling::token() const noexcept
{
    return BinaryConstView(*m_token);
}

const char *Handling::NAckReasonToString(NAckReason reason) noexcept
{
    switch(reason){
    case TooManyRetransmit: return "Too Many Retransmit-消息已被重传多次，但未收到确认";
    case NotDelivered: return "Not Delivered-报文无法发送，通常是因为无法到达目的地或者报文格式或内容有错误。";
    case Reset: return "Reset-从接收方收到一条重置消息（RST）作为对消息的响应。这是一个收件人不理解报文或者无法处理报文的信号";
    case TlsFailed: return "Tls Failed-TLS（传输层安全）级别的失败，例如握手失败，证书无效，或者密码套件不匹配。";
    case IcmpIssue: return "Icmp Issue-收到了一条ICMP（Internet控制报文协议）错误报文，例如 \"目标不可达 \"或 \"超过时间\"。";
    case BadResponse: return "Bad Response-收到了一个意外的或无效的响应，例如一个畸形的数据包或一个代码不正确的响应。";
    case TlsLayerFailure: return "Tls Layer Failure-传输层安全失败";
    case WsLayerFailure: return "Ws Layer Failure-表示 WebSocket 层出现故障。";
    case WsFailure: return "Ws Failure-未能建立连接或连接意外关闭。";
    default: return "Unknown";
    }
}

Handling::Handling(int mid, BinaryConst token) noexcept
    : m_mid(mid), m_token(new BinaryConst(std::move(token)))
{
}

Handling::~Handling() noexcept
{
    if(m_token)
        delete m_token;
}

} // namespace CoapPlusPlus