#include <coap3/coap.h> 
#include "Resource.h"
#include "coap/Session.h"
#include "ResourceInterface.h"
#include "coap/exception.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/ResponsePdu.h"

#include <string>
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

std::string Resource::getUriPath() const noexcept
{
    auto str = coap_resource_get_uri_path(m_resource);
    return m_uriPath;
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

void Resource::registerInterface(std::unique_ptr<ResourceInterface> resourceInterface)
{ 
    if(!resourceInterface)
        throw std::invalid_argument("resourceInterface is nullptr");
    if(m_resourceInterface.find(resourceInterface->code()) != m_resourceInterface.end())
        throw AlreadyExistException("resourceInterface already exist");
    auto code = resourceInterface->code();
    m_resourceInterface.insert(std::make_pair(code, resourceInterface.release()));
}


void Resource::getRequestCallback(coap_resource_t* resource, coap_session_t* session,
    const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response)
{
    // 处理 GET 请求
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    if(resourceWrapper != nullptr) {
        try {
            auto imp = resourceWrapper->getResourceInterface(Information::RequestCode::Get);
            
            auto string = query == nullptr ? "" : std::string((const char*)query->s, query->length);
            auto token = coap_pdu_get_token(request); // 如果请求没有token，虽然长度为0的，data是null但是token不为空，所以BinaryConst::Create不会抛出异常
            imp->onRequest(Session(session,false), string, response, RequestPdu(const_cast<coap_pdu_t*>(request), BinaryConst::Create(token.length, token.s)));
        } catch(std::exception& e) {
            coap_log_warn("Resource::getRequestCallback error: %s, path:%s\n", e.what(), resourceWrapper->getUriPath().c_str());
            coap_pdu_set_code(response, static_cast<coap_pdu_code_t>(Information::NotImplemented));
        }
    }
    coap_show_pdu(LOG_DEBUG, request);
    coap_show_pdu(LOG_DEBUG, response);
}

void Resource::putRequestCallback(coap_resource_t* resource, coap_session_t* session,
   const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response)
{
    // 处理 PUT 请求
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    if(resourceWrapper != nullptr) {
        try {
            auto imp = resourceWrapper->getResourceInterface(Information::RequestCode::Put);
            auto string = query == nullptr ? "" : std::string((const char*)query->s, query->length);
            auto token = coap_pdu_get_token(request);
            imp->onRequest(session, string, response, RequestPdu(const_cast<coap_pdu_t*>(request), BinaryConst::Create(token.length, token.s)));
        } catch(std::exception& e) {
            coap_log_warn("Resource::putRequestCallback error: %s, path:%s\n", e.what(), resourceWrapper->getUriPath().c_str());
            coap_pdu_set_code(response, static_cast<coap_pdu_code_t>(Information::NotImplemented));
        }
    }
    coap_show_pdu(LOG_DEBUG, request);
    coap_show_pdu(LOG_DEBUG, response);
}

void Resource::postRequestCallback(coap_resource_t* resource, coap_session_t* session,
    const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response)
{
    // 处理 POST 请求
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    if(resourceWrapper != nullptr) {
        try {
            auto imp = resourceWrapper->getResourceInterface(Information::RequestCode::Post);
            auto string = query == nullptr ? "" : std::string((const char*)query->s, query->length);
            auto token = coap_pdu_get_token(request);
            imp->onRequest(session, string, response, RequestPdu(const_cast<coap_pdu_t*>(request), BinaryConst::Create(token.length, token.s)));
        } catch(std::exception& e) {
            coap_log_warn("Resource::postRequestCallback error: %s, path:%s\n", e.what(), resourceWrapper->getUriPath().c_str());
            coap_pdu_set_code(response, static_cast<coap_pdu_code_t>(Information::NotImplemented));
        }
    }
    coap_show_pdu(LOG_DEBUG, request);
    coap_show_pdu(LOG_DEBUG, response);
}

void Resource::deleteRequestCallback(coap_resource_t* resource, coap_session_t* session,
    const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response)
{
    // 处理 DELETE 请求
    auto resourceWrapper = static_cast<Resource*>(coap_resource_get_userdata(resource));
    if(resourceWrapper != nullptr) {
        try {
            auto imp = resourceWrapper->getResourceInterface(Information::RequestCode::Delete);
            auto string = query == nullptr ? "" : std::string((const char*)query->s, query->length);
            auto token = coap_pdu_get_token(request);
            imp->onRequest(session, string, response, RequestPdu(const_cast<coap_pdu_t*>(request), BinaryConst::Create(token.length, token.s)));
        } catch(std::exception& e) {
            coap_log_warn("Resource::deleteRequestCallback error: %s, path:%s\n", e.what(), resourceWrapper->getUriPath().c_str());
            coap_pdu_set_code(response, static_cast<coap_pdu_code_t>(Information::NotImplemented));
        }
    }
    coap_show_pdu(LOG_DEBUG, request);
    coap_show_pdu(LOG_DEBUG, response);
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
    coap_register_request_handler(m_resource, COAP_REQUEST_GET, Resource::getRequestCallback);
    coap_register_request_handler(m_resource, COAP_REQUEST_PUT, Resource::putRequestCallback);
    coap_register_request_handler(m_resource, COAP_REQUEST_POST, Resource::postRequestCallback);
    coap_register_request_handler(m_resource, COAP_REQUEST_DELETE, Resource::deleteRequestCallback);

    coap_resource_set_userdata(m_resource, this);
    coap_resource_set_get_observable(m_resource, m_observable ? 1 : 0);
    m_isInit = true;
    getUriPath();// 00000000000000000000000
    return true;
}

void Resource::freeResource() noexcept
{
    if (m_resource) {
        coap_resource_set_userdata(m_resource, nullptr);
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

} // namespace CoapPlusPlus
