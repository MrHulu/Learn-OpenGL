#pragma once


namespace CoapPlusPlus {
namespace Information {

enum MessageType {
    Confirmable = 0,
    NonConfirmable = 1,
    Acknowledgement = 2,
    Reset = 3
};

enum ResponseCode {
    Created = 65,
    Deleted = 66,
    Valid = 67,
    Changed = 68,
    Content = 69,
    Continue = 95,
    BadRequest = 128,
    Unauthorized = 129,
    BadOption = 130,
    Forbidden = 131,
    NotFound = 132,
    MethodNotAllowed = 133,
    NotAcceptable = 134,
    PreconditionFailed = 140,
    RequestEntityTooLarge = 141,
    UnsupportedContentFormat = 143,
    InternalServerError = 160,
    NotImplemented = 161,
    BadGateway = 162,
    ServiceUnavailable = 163,
    GatewayTimeout = 164,
    ProxyingNotSupported = 165,
    HopLimitReached = 166
};


enum RequestCode {
    Get = 1,
    Post = 2,
    Put = 3,
    Delete = 4,
    Fetch = 5,
    Patch = 6,
    IPatch = 7
};

};// namespace Information
};// namespace CoapPlusPlus
