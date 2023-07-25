#include "Resource.h"
#include "ResourceInterface.h"
#include "coap/exception.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"

namespace CoapPlusPlus
{

Resource::Resource(const std::string& uriPath, bool observable) noexcept
    : m_uriPath(uriPath), m_observable(observable)
{
    
}

Resource::~Resource() noexcept
{
    freeResource();
    for(auto iter = m_resourceInterface.begin(); iter != m_resourceInterface.end(); ++iter) {
        delete iter->second;
    }
}

bool Resource::enableNotificationMessageConfirmable(bool enable) noexcept
{
    if(!m_resource)
        return false;
    // 设置通知消息类型
    coap_resource_set_mode(m_resource, enable ? COAP_RESOURCE_FLAGS_NOTIFY_CON : COAP_RESOURCE_FLAGS_NOTIFY_NON);
    return true;
}

void Resource::notifyObserver() noexcept
{
    // 通知所有观察者
    if(m_resource)
        coap_resource_notify_observers(m_resource, nullptr);
}

void Resource::registerInterface(std::unique_ptr<ResourceInterface> resourceInterface) noexcept
{ 
    // 注册资源接口
    m_resourceInterface[resourceInterface->code()] = resourceInterface.release();
}

void Resource::getRequestCallback(coap_resource_t* resource, coap_session_t* session,
    const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response)
{
    // 处理 GET 请求
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    try {
        auto imp = resourceWrapper->getResourceInterface(Information::RequestCode::Get);
        
        //imp->onRequest(session, request, query, response);
    } catch(...) {
        coap_pdu_set_code(response, static_cast<coap_pdu_code_t>(Information::NotImplemented));
    }
    logPdu(const_cast<coap_pdu_t*>(request), response);
}

void Resource::putRequestCallback(coap_resource_t* resource, coap_session_t* session,
   const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response)
{
    // 处理 PUT 请求
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    try {
        auto imp = resourceWrapper->getResourceInterface(Information::RequestCode::Put);
        // TODO: 参数未实现
        //imp->onRequest(session, request, query, response);
    } catch(...) {
        coap_pdu_set_code(response, static_cast<coap_pdu_code_t>(Information::NotImplemented));
    }
    logPdu(const_cast<coap_pdu_t*>(request), response);
}

void Resource::postRequestCallback(coap_resource_t* resource, coap_session_t* session,
    const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response)
{
    // 处理 POST 请求
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    try {
        auto imp = resourceWrapper->getResourceInterface(Information::RequestCode::Post);
    } catch(...) {
        coap_pdu_set_code(response, static_cast<coap_pdu_code_t>(Information::NotImplemented));
    }
    logPdu(const_cast<coap_pdu_t*>(request), response);
}

void Resource::deleteRequestCallback(coap_resource_t* resource, coap_session_t* session,
    const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response)
{
    // 处理 DELETE 请求
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    try {
        auto imp = resourceWrapper->getResourceInterface(Information::RequestCode::Delete);
        //imp->onRequest(session, request, query, response);
    } catch(...) {
        coap_pdu_set_code(response, static_cast<coap_pdu_code_t>(Information::NotImplemented));
    }
    logPdu(const_cast<coap_pdu_t*>(request), response);
}

bool Resource::initResource() noexcept
{
    if(m_isInit == true) {
        return true;
    }
    // 创建 coap_resource_t
    m_resource = coap_resource_init(coap_make_str_const(m_uriPath.c_str()), 0);
    if(!m_resource) {
        return false;
    }

    // 设置回调函数
    coap_register_handler(m_resource, COAP_REQUEST_GET, Resource::getRequestCallback);
    coap_register_handler(m_resource, COAP_REQUEST_PUT, Resource::putRequestCallback);
    coap_register_handler(m_resource, COAP_REQUEST_POST, Resource::postRequestCallback);
    coap_register_handler(m_resource, COAP_REQUEST_DELETE, Resource::deleteRequestCallback);

    coap_resource_set_userdata(m_resource, this);
    coap_resource_set_get_observable(m_resource, m_observable ? 1 : 0);
    m_isInit = true;
    return true;
}

void Resource::freeResource() noexcept
{
    if (m_resource) {
        coap_delete_resource(nullptr, m_resource);
        m_resource = nullptr;
    }
}

ResourceInterface *Resource::getResourceInterface(Information::RequestCode requestCode)
{
    auto iter = m_resourceInterface.find(requestCode);
    if(iter == m_resourceInterface.end()) {
        throw TargetNotFoundException("ResourceInterface not found");
    }
    return iter->second;
}

void Resource::logPdu(coap_pdu_t *request, coap_pdu_t *response) noexcept
{
    if (request && response) {
        auto raw_token = coap_pdu_get_token(request);
        if(raw_token.length > 0 && raw_token.s != nullptr) {
            RequestPdu requestPdu(request, BinaryConst::Create(raw_token.length, raw_token.s));
            ResponsePdu responsePdu(response);
            Pdu::LogPdu(COAP_LOG_DEBUG, &requestPdu);
            Pdu::LogPdu(COAP_LOG_DEBUG, &responsePdu);
        }
    }
}

} // namespace CoapPlusPlus
