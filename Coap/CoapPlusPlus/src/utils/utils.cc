#include "Utils.h"

namespace CoapPlusPlus {

int getResponseMasterStatus(coap_pdu_code_t code) { return COAP_RESPONSE_CLASS(code); }

int getResponseSubStatus(coap_pdu_code_t code) { return code & 0x1F; }

int getResponseStatus(coap_pdu_code_t code) {
    switch (code) {
    case COAP_RESPONSE_CODE(0):
        return COAP_RESPONSE_EMPTY;
    case COAP_RESPONSE_CODE(201):
        return COAP_RESPONSE_CREATED;
    case COAP_RESPONSE_CODE(202):
        return COAP_RESPONSE_DELETED;
    case COAP_RESPONSE_CODE(203):
        return COAP_RESPONSE_VALID;
    case COAP_RESPONSE_CODE(204):
        return COAP_RESPONSE_CHANGED;
    case COAP_RESPONSE_CODE(205):
        return COAP_RESPONSE_CONTENT;
    case COAP_RESPONSE_CODE(400):
        return COAP_RESPONSE_BAD_REQUEST;
    case COAP_RESPONSE_CODE(401):
        return COAP_RESPONSE_UNAUTHORIZED;
    case COAP_RESPONSE_CODE(402):
        return COAP_RESPONSE_BAD_OPTION;
    case COAP_RESPONSE_CODE(403):
        return COAP_RESPONSE_FORBIDDEN;
    case COAP_RESPONSE_CODE(404):
        return COAP_RESPONSE_NOT_FOUND;
    case COAP_RESPONSE_CODE(405):
        return COAP_RESPONSE_METHOD_NOT_ALLOWED;
    case COAP_RESPONSE_CODE(406):
        return COAP_RESPONSE_NOT_ACCEPTABLE;
    case COAP_RESPONSE_CODE(412):
        return COAP_RESPONSE_PRECONDITION_FAILED;
    case COAP_RESPONSE_CODE(413):
        return COAP_RESPONSE_REQUEST_ENTITY_TOO_LARGE;
    case COAP_RESPONSE_CODE(415):
        return COAP_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
    case COAP_RESPONSE_CODE(500):
        return COAP_RESPONSE_INTERNAL_SERVER_ERROR;
    case COAP_RESPONSE_CODE(501):
        return COAP_RESPONSE_NOT_IMPLEMENTED;
    case COAP_RESPONSE_CODE(502):
        return COAP_RESPONSE_BAD_GATEWAY;
    case COAP_RESPONSE_CODE(503):
        return COAP_RESPONSE_SERVICE_UNAVAILABLE;
    case COAP_RESPONSE_CODE(504):
        return COAP_RESPONSE_GATEWAY_TIMEOUT;
    case COAP_RESPONSE_CODE(505):
        return COAP_RESPONSE_PROXYING_NOT_SUPPORTED;
    default:
        return COAP_RESPONSE_UNKNOWN;
    }
}

std::string getResponseStatusString(coap_pdu_code_t code) {
    switch (code) {
    case COAP_RESPONSE_CODE(0):
        return "Empty";
    case COAP_RESPONSE_CODE(201):
        return "Created";
    case COAP_RESPONSE_CODE(202):
        return "Deleted";
    case COAP_RESPONSE_CODE(203):
        return "Valid";
    case COAP_RESPONSE_CODE(204):
        return "Changed";
    case COAP_RESPONSE_CODE(205):
        return "Content";
    case COAP_RESPONSE_CODE(231):
        return "Continue";
    case COAP_RESPONSE_CODE(400):
        return "Bad Request";
    case COAP_RESPONSE_CODE(401):
        return "Unauthorized";
    case COAP_RESPONSE_CODE(402):
        return "Bad Option";
    case COAP_RESPONSE_CODE(403):
        return "Forbidden";
    case COAP_RESPONSE_CODE(404):
        return "Not Found";
    case COAP_RESPONSE_CODE(405):
        return "Method Not Allowed";
    case COAP_RESPONSE_CODE(406):
        return "Not Acceptable";
    case COAP_RESPONSE_CODE(408):
        return "Request Entity Incomplete";
    case COAP_RESPONSE_CODE(412):
        return "Precondition Failed";
    case COAP_RESPONSE_CODE(413):
        return "Request Entity Too Large";
    case COAP_RESPONSE_CODE(415):
        return "Unsupported Content-Format";
    case COAP_RESPONSE_CODE(500):
        return "Internal Server Error";
    case COAP_RESPONSE_CODE(501):
        return "Not Implemented";
    case COAP_RESPONSE_CODE(502):
        return "Bad Gateway";
    case COAP_RESPONSE_CODE(503):
        return "Service Unavailable";
    case COAP_RESPONSE_CODE(504):
        return "Gateway Timeout";
    case COAP_RESPONSE_CODE(505):
        return "Proxying Not Supported";
    default:
        return "Unkown";
    }
}


}// namespace CoapPlusPlus 