#include "Resource.h"
#include "SimpleServer.h"
#include "coap/exception.h"
#include <iostream>


namespace Coap {

Resource::~Resource() 
{
    // TODO: 需不需要删除呢？因为Context对象会删除所有的资源对象
     freeResource();
}

Resource::Resource(const std::string& uriPath, bool observable)
    : m_uriPath(uriPath)
{
    m_resource = coap_resource_init(coap_make_str_const(m_uriPath.c_str()), 0);
    if(m_resource == nullptr)
        throw CallCoapLibFuncException("coap_resource_init failed");
    
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

void Resource::freeResource() noexcept
{
    // 通知上级服务器删除该资源， @todo： 没有信号槽机制只能出此下策(使用友元类方法内部封装的方式)
    if(server) {
        server->m_resources.erase(m_uriPath);
    }
    if(m_resource) {
        coap_delete_resource(nullptr, m_resource);
        m_resource = nullptr;
    }
}

void Resource::getRequestCallbackWrapper(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));

    // create a new session for this request
    resourceWrapper->getRequestCallback(session, request, query, response);
}

void Resource::postRequestCallbackWrapper(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    resourceWrapper->postRequestCallback(session, request, query, response);
}

void Resource::putRequestCallbackWrapper(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    resourceWrapper->putRequestCallback(session, request, query, response);
}

void Resource::deleteRequestCallbackWrapper(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response)
{
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    resourceWrapper->deleteRequestCallback(session, request, query, response);
}


}; // namespace Coap