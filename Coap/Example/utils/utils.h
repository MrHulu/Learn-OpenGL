#pragma once

#include <coap3/coap.h>

namespace Coap {



/**
 *  @brief 得到一个Coap响应的状态
 *  @param code 响应的code
 *  @return 响应的状态
 *      @c 0 表示空报文
 *      @c 2 表示正确响应
 *      @c 4 表示客户端错误
 *      @c 5 表示服务端错误
*/
int getResponseStatus(coap_pdu_code_t code) { return COAP_RESPONSE_CLASS(code); }

/**
 *  @brief 从响应的code得到一个CoAP请求方法
 *  @param code 响应的code
 *  @return CoAP请求方法
 *      @c 1 表示GET方法
 *      @c 2 表示POST方法
 *      @c 3 表示PUT方法
 *      @c 4 表示DELETE方法
*/
int getRequestMethod(coap_pdu_code_t code) { return code & 0x1F; }

};