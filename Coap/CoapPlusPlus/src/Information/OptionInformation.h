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
static const char* OptionNumberToString(OptionNumber number) noexcept
{
    switch (number)
    {
    case OptionNumber::IfMatch:
        return "If-Match";
    case OptionNumber::UriHost:
        return "Uri-Host";
    case OptionNumber::ETag:
        return "ETag";
    case OptionNumber::IfNoneMatch:
        return "If-None-Match";
    case OptionNumber::Observe:
        return "Observe";
    case OptionNumber::UriPort:
        return "Uri-Port";
    case OptionNumber::LocationPath:
        return "Location-Path";
    case OptionNumber::UriPath:
        return "Uri-Path";
    case OptionNumber::ContentFormat:
        return "Content-Format";
    case OptionNumber::MaxAge:
        return "Max-Age";
    case OptionNumber::UriQuery:
        return "Uri-Query";
    case OptionNumber::HopLimit:
        return "Hop-Limit";
    case OptionNumber::Accept:
        return "Accept";
    case OptionNumber::LocationQuery:
        return "Location-Query";
    case OptionNumber::Block2:
        return "Block2";
    case OptionNumber::Block1:
        return "Block1";
    case OptionNumber::Size2:
        return "Size2";
    case OptionNumber::ProxyUri:
        return "Proxy-Uri";
    case OptionNumber::ProxyScheme:
        return "Proxy-Scheme";
    case OptionNumber::Size1:
        return "Size1";
    case OptionNumber::Echo:
        return "Echo";
    case OptionNumber::NoResponse:
        return "No-Response";
    case OptionNumber::RTag:
        return "RTag";
    default:
        return "unknown";
    }
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
static const char* ContentFormatTypeToString(ContentFormatType type) noexcept 
{
    switch (type)
    {
    case ContentFormatType::TextPlain:
        return "text/plain";
    case ContentFormatType::LinkFormat:
        return "application/link-format";
    case ContentFormatType::Xml:
        return "application/xml";
    case ContentFormatType::OctetStream:
        return "application/octet-stream";
    case ContentFormatType::RdfXml:
        return "application/rdf+xml";
    case ContentFormatType::Exi:
        return "application/exi";
    case ContentFormatType::Json:
        return "application/json";
    case ContentFormatType::Cbor:
        return "application/cbor";
    case ContentFormatType::CWT:
        return "application/cwt";
    default:
        return "unknown";
    }
};

};// namespace Information
};// namespace CoapPlusPlus