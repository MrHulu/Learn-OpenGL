#pragma once

#include "Resource.h"

namespace CoapPlusPlus {

class TextResource : public Resource 
{
public:
    TextResource(const std::string& uriPath, bool observable = false) 
        : Resource(uriPath, observable) {  }

public:
    virtual void onUpdateTextFromClient(const std::string& text) noexcept = 0;
    virtual std::string text() const noexcept = 0;
protected:
    void getRequestCallback(coap_session_t *session, 
        const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override;
    void putRequestCallback(coap_session_t *session, 
        const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override;
    void postRequestCallback(coap_session_t *session,
        const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override;
    void deleteRequestCallback(coap_session_t *session,
        const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override;
    
};


};// namespace CoapPlusPlus 