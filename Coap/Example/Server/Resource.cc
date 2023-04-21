#include "Resource.h"
#include <iostream>
namespace Coap {

Resource::~Resource() 
{
    // TODO: 貌似不需要这个，因为coap_free_context会释放所有资源
    //coap_delete_resource(m_context, m_resource->key);
}

void Resource::getRequestCallbackWrapper(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    resourceWrapper->getRequestCallback(resource, session, request, query, response);
}

void Resource::postRequestCallbackWrapper(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    resourceWrapper->postRequestCallback(resource, session, request, query, response);
}

void Resource::putRequestCallbackWrapper(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    resourceWrapper->putRequestCallback(resource, session, request, query, response);
}

void Resource::deleteRequestCallbackWrapper(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    resourceWrapper->deleteRequestCallback(resource, session, request, query, response);
}

Resource::Resource(coap_context_t* context, const std::string& uriPath, bool observable)
    : m_context(context), m_uriPath(uriPath) 
{
    m_resource = coap_resource_init(coap_make_str_const(m_uriPath.c_str()), 0);
    coap_resource_set_get_observable(m_resource, observable ? 1 : 0);
    coap_resource_set_userdata(m_resource, this);
    // TODO: Resource被context管理，这里不需要释放
    // coap_resource_release_userdata_handler(m_context, [](void *data){
    //     auto resource = static_cast<Resource*>(data);
    //     if(resource)
    //         delete resource;
    // });
    coap_register_handler(m_resource, COAP_REQUEST_GET, getRequestCallbackWrapper);
    coap_register_handler(m_resource, COAP_REQUEST_POST, postRequestCallbackWrapper);
    coap_register_handler(m_resource, COAP_REQUEST_PUT, putRequestCallbackWrapper);
    coap_register_handler(m_resource, COAP_REQUEST_DELETE, deleteRequestCallbackWrapper);
}

void Resource::notifyObservers() noexcept
{
    coap_resource_notify_observers(m_resource, nullptr);
}

}; // namespace Coap