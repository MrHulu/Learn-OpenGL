#pragma once

#include <coap3/coap.h>
#include <string>
namespace Coap {


enum  CoapResponse : int {
    COAP_RESPONSE_UNKNOWN = -1,
    COAP_RESPONSE_EMPTY = 0,
    COAP_RESPONSE_CREATED = 201,
    COAP_RESPONSE_DELETED = 202,
    COAP_RESPONSE_VALID = 203,
    COAP_RESPONSE_CHANGED = 204,
    COAP_RESPONSE_CONTENT = 205,
    COAP_RESPONSE_BAD_REQUEST = 400,
    COAP_RESPONSE_UNAUTHORIZED = 401,
    COAP_RESPONSE_BAD_OPTION = 402,
    COAP_RESPONSE_FORBIDDEN = 403,
    COAP_RESPONSE_NOT_FOUND = 404,
    COAP_RESPONSE_METHOD_NOT_ALLOWED = 405,
    COAP_RESPONSE_NOT_ACCEPTABLE = 406,
    COAP_RESPONSE_PRECONDITION_FAILED = 412,
    COAP_RESPONSE_REQUEST_ENTITY_TOO_LARGE = 413,
    COAP_RESPONSE_UNSUPPORTED_CONTENT_FORMAT = 415,
    COAP_RESPONSE_INTERNAL_SERVER_ERROR = 500,
    COAP_RESPONSE_NOT_IMPLEMENTED = 501,
    COAP_RESPONSE_BAD_GATEWAY = 502,
    COAP_RESPONSE_SERVICE_UNAVAILABLE = 503,
    COAP_RESPONSE_GATEWAY_TIMEOUT = 504,
    COAP_RESPONSE_PROXYING_NOT_SUPPORTED = 505
};


/**
 * @brief 客户端从响应的code得到一个Coap主状态值
 * @param code 响应的code
 * @return 响应的主状态值
 *     @c 0 表示空报文，收到一个ACK或者RST报文，或者收到一个请求报文，但是没有找到对应的资源。
 *     @c 2 表示正确响应，请求已成功被服务器接收、理解、并接受。
 *     @c 4 表示客户端错误，客户端看起来可能发生了错误，妨碍了服务器的处理。
 *     @c 5 表示服务端错误，在处理请求的过程中有错误或者异常状态发生或者当前的软硬件资源无法完成对请求的处理。
 * @see getResponseSubStatus(coap_pdu_code_t code)
 * @see getResponseStatus(coap_pdu_code_t code)
 * @see getResponseStatusString(coap_pdu_code_t code)
*/
int getResponseMasterStatus(coap_pdu_code_t code);

/**
 * @brief 客户端从响应的code得到一个CoAP子状态值
 * @param code 响应的code
 * @return 响应的子状态值
 * @see getResponseMasterStatus(coap_pdu_code_t code)
 * @see getResponseStatus(coap_pdu_code_t code)
 * @see getResponseStatusString(coap_pdu_code_t code)
*/
int getResponseSubStatus(coap_pdu_code_t code);


/**
 * @brief 客户端从响应的code得到一个Coap状态值
 * 
 * @param code 响应的code 
 * @return Coap状态
 *      201 Created（已创建）：资源已成功创建。
 *      202 Deleted（已删除）：资源已成功删除。
 *      203 Valid（有效）：请求已验证，但不会返回新数据。
 *      204 Changed（已更改）：资源已成功更新。
 *      205 Content（内容）：请求成功，响应消息包含所请求的内容。
 *      400 Bad Request（错误请求）：请求无效或不完整。
 *      401 Unauthorized（未授权）：请求需要认证。
 *      402 Bad Option（错误选项）：请求包含无法识别的选项。
 *      403 Forbidden（禁止）：请求目标无权访问。
 *      404 Not Found（未找到）：请求的资源不存在。
 *      405 Method Not Allowed（方法不允许）：请求方法不允许使用。
 *      406 Not Acceptable（不可接受）：请求的内容格式不受服务器支持。
 *      412 Precondition Failed（前提条件失败）：请求的条件未满足。
 *      413 Request Entity Too Large（请求实体过大）：请求的实体大小超出服务器处理能力。
 *      415 Unsupported Content-Format（不支持的内容格式）：请求的内容格式不受服务器支持。
 *      500 Internal Server Error（内部服务器错误）：服务器无法处理请求。
 *      501 Not Implemented（未实现）：服务器不支持请求的功能。
 *      502 Bad Gateway（错误网关）：服务器作为网关或代理，从上游服务器收到无效响应。
 *      503 Service Unavailable（服务不可用）：服务器无法处理请求，可能是由于过载或维护。
 *      504 Gateway Timeout（网关超时）：服务器作为网关或代理，但没有及时从上游服务器收到响应。
 *      505 Proxying Not Supported（代理不支持）：服务器不支持代理。
 * @see getResponseMasterStatus(coap_pdu_code_t code)
 * @see getResponseSubStatus(coap_pdu_code_t code)
 * @see getResponseStatusString(coap_pdu_code_t code)
 */
int getResponseStatus(coap_pdu_code_t code) ;

/**
 * @brief 客户端从响应的code得到一个Coap状态值
 * 
 * @param code 响应的code 
 * @return 得到一个Coap状态的含义的字符串
 * @see getResponseMasterStatus(coap_pdu_code_t code)
 * @see getResponseSubStatus(coap_pdu_code_t code)
 * @see getResponseStatus(coap_pdu_code_t code)
 */ 
std::string getResponseStatusString(coap_pdu_code_t code);


}; // namespace Coap