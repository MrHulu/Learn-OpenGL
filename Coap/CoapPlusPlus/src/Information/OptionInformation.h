#pragma once

namespace CoapPlusPlus {
namespace Information
{

enum OptionNumber {
    IfMatch = 1,
    UriHost = 3,
    ETag = 4,
    IfNoneMatch = 5,
    Observe = 6,
    UriPort = 7,
    LocationPath = 8,
    UriPath = 11,
    ContentFormat = 12,
    MaxAge = 14,
    UriQuery = 15,
    HopLimit = 16,
    Accept = 17,
    LocationQuery = 20,
    Block2 = 23,
    Block1 = 27,
    Size2 = 28,
    ProxyUri = 35,
    ProxyScheme = 39,
    Size1 = 60,
    Echo = 252,
    NoResponse = 258,
    RTag = 292,
    MaxOpt = 65535
};

enum ContentFormatType {
    NoneType = -1,
    TextPlain = 0,
    LinkFormat = 40,
    Xml = 41,
    OctetStream = 42,
    RdfXml = 43,
    Exi = 47,
    Json = 50,
    Cbor = 60,
    CWT = 61
};

};// namespace Information
};// namespace CoapPlusPlus