#include "TextResource.h"
#include "coap/BlockTransferReader.h"

namespace CoapPlusPlus {

void TextResource::getRequestCallback(coap_session_t *session, 
    const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    /*
    * request will be NULL if an Observe triggered request, so the uri_path,
    * if needed, must be abstracted from the resource.
    * The uri_path string is a const pointer
    * 如果请求的资源被删除，通常情况下，回调函数不会被执行。
    * 但是，如果客户端之前订阅了该资源（带有Observe选项的请求），并且在资源被删除之后，
    * 服务器仍然尝试触发Observe通知，则可能会导致回调函数被执行。
    * 在这种情况下，对uri_path的检查有助于发现资源是否仍然存在，以便返回一个“未找到”的响应。
    */
    auto uri_path = coap_resource_get_uri_path(getResource());
    if (!uri_path) {
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_NOT_FOUND);
        return;
    }
    coap_pdu_set_type(response, COAP_MESSAGE_CON);
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
    
    uint8_t buf[4];
    auto formatOptionLen = coap_encode_var_safe(buf, sizeof(buf), COAP_MEDIATYPE_TEXT_PLAIN);
    coap_add_option(response, COAP_OPTION_CONTENT_FORMAT, formatOptionLen, buf);

    // 使用coap_add_data_large_response()函数，可以自动实现块传输
    auto temp = text();
    coap_add_data_large_response(getResource(), session, request, response, query, 
                                    COAP_MEDIATYPE_TEXT_PLAIN, -1, 0, temp.length(),
                                    reinterpret_cast<const uint8_t*>(temp.c_str()), 
                                    nullptr, nullptr);

}

void TextResource::putRequestCallback(coap_session_t *session, 
    const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto urlPath = coap_get_uri_path(request);
    if(urlPath == nullptr) {
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_NOT_FOUND);
        std::string urlPathString = std::string(reinterpret_cast<const char*>(urlPath->s), urlPath->length);
        std::string msg = urlPathString + std::string(" urlPath is invalid");
        coap_log(LOG_DEBUG, msg.c_str());
        return;
    }
    coap_delete_string(urlPath);
    try {
        BlockTransferReader reader(session, request);
        auto data = reader.read();
        std::string text(reinterpret_cast<const char*>(data->s), data->length);
        onUpdateTextFromClient(text);
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);
        notifyObservers();
        coap_delete_binary(data);
    }
    catch(DataNotReadyException& e) {
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTINUE);
        std::string msg = e.what();
        coap_log(LOG_WARNING, msg.c_str());
        return;
    }
    catch(std::exception& e) {
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
        std::string msg = e.what();
        coap_log(LOG_WARNING, msg.c_str());
        return;

    }
}

void TextResource::postRequestCallback(coap_session_t *session, 
    const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_NOT_ALLOWED);
    std::string msg = "POST method is not allowed";
    coap_add_data(response, msg.length(), reinterpret_cast<const uint8_t*>(msg.c_str()));
}

void TextResource::deleteRequestCallback(coap_session_t *session, 
    const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_NOT_ALLOWED);
    std::string msg = "DELE method is not allowed";
    coap_add_data(response, msg.length(), reinterpret_cast<const uint8_t*>(msg.c_str()));
}

};// namespace CoapPlusPlus 