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
    Tls = 4
};


enum SessionState {
    None = 0,
    Connecting = 1,
    Handshaking = 2,
    Csm = 3,
    Established = 4
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

