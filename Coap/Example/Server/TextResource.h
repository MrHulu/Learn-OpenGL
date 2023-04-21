#pragma once

#include "Resource.h"
namespace Coap {

class TextResource : public Resource 
{
public:
    TextResource(coap_context_t* context, const std::string& uriPath, bool observable = false) 
        : Resource(context, uriPath, observable) {  }

public:
    virtual void onUpdateTextFromClient(const std::string& text) noexcept = 0;
    virtual std::string text() const noexcept = 0;
protected:
    void getRequestCallback(coap_resource_t *resource, coap_session_t *session, 
        const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override;
    void putRequestCallback(coap_resource_t *resource, coap_session_t *session, 
        const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override;
    void postRequestCallback(coap_resource_t *resource, coap_session_t *session,
        const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override;
    void deleteRequestCallback(coap_resource_t *resource, coap_session_t *session,
        const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override;
    
};


}; // namespace Coap