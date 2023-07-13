#pragma once
namespace CoapPlusPlus
{
namespace Information
{

enum Protocol {
    None = 0,
    Udp = 1,
    Dtls = 2,
    Tcp = 3,
    Tls = 4,
    Websocket = 5,
    WebsocketSecure = 6,
    Last = 7
};


enum SessionState {
    NoneState = 0,      // 这可能是会话的初始状态，在尝试任何连接之前。
    Connecting = 1,     // 会话正在与CoAP服务器建立连接。这可能涉及到网络操作，如DNS查询、TCP握手，或为安全CoAP（CoAPs）会话启动DTLS握手。
    Handshaking = 2,    // 对于安全的CoAPs会话，在连接建立后，会进行加密握手（如DTLS握手）。该状态表示当前正在进行握手。
    Csm = 3,            // 在通过可靠传输（如TCP或WebSockets）的CoAP协议中，有一个握手称为能力和设置消息（CSM）交换。如果会话处于这种状态，就意味着CSM交换正在进行。
    Established = 4     // 会话已成功建立，现在可以进行CoAP消息交换。所有必要的握手和交换都已完成，会话已完全正常运行。
};


enum NAckReason {
    TooManyRetransmissions = 0,
    NotDeliverable = 1,
    RST = 2,
    TLSFailed = 3,
    ICMPError = 4,
    BadResponse = 5,
    TLSLayerFailure = 6,
    WSLayerFailure = 7,
    WSFailure = 8,
};


}// namespace Information
}// namespace CoapPlusPlus

