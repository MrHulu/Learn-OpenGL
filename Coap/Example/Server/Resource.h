#pragma once

#include <coap3/coap.h>
#include <string>

namespace Coap {

class Resource 
{
public:
    virtual ~Resource();
    coap_resource_t* getResource() noexcept { return m_resource; }
    std::string getUriPath() const noexcept { return m_uriPath; }
private:
    static void getRequestCallbackWrapper(coap_resource_t* resource, coap_session_t* session,
                                        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void postRequestCallbackWrapper(coap_resource_t* resource, coap_session_t* session,
                                        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void putRequestCallbackWrapper(coap_resource_t* resource, coap_session_t* session,
                                        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void deleteRequestCallbackWrapper(coap_resource_t* resource, coap_session_t* session,
                                        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);

protected:
    Resource(coap_context_t* context, const std::string& uriPath, bool observable = false);
    void notifyObservers() noexcept;

    virtual void getRequestCallback(coap_resource_t* resource, coap_session_t* session,
                                const coap_pdu_t* request, const coap_string_t* query, 
                                coap_pdu_t* response) = 0;
    virtual void postRequestCallback(coap_resource_t* resource, coap_session_t* session,
                                const coap_pdu_t* request, const coap_string_t* query, 
                                coap_pdu_t* response) = 0;
    virtual void putRequestCallback(coap_resource_t* resource, coap_session_t* session,
                                const coap_pdu_t* request, const coap_string_t* query, 
                                coap_pdu_t* response) = 0;
    virtual void deleteRequestCallback(coap_resource_t* resource, coap_session_t* session,
                                const coap_pdu_t* request, const coap_string_t* query, 
                                coap_pdu_t* response) = 0;

protected:
    coap_context_t* m_context;
    coap_resource_t* m_resource;
    std::string m_uriPath;
};

}; // namespace Coap