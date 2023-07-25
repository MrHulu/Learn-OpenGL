/**
 * @file Resource.h
 * @author Hulu
 * @brief coap_resource_t 的C++封装类定义
 * @version 0.1
 * @date 2023-06-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "coap/Information/PduInformation.h"
#include <string>
#include <memory>
#include <map>

struct coap_resource_t;
struct coap_session_t;
struct coap_pdu_t;
struct coap_string_t;
namespace CoapPlusPlus
{
class ResourceInterface;
class Resource
{
    friend class ResourceManager;
    Resource& operator=(const Resource&) = delete;
    Resource& operator=(Resource&&) = delete;
    Resource(const Resource&) = delete;
    Resource(Resource&&) = delete;
public:
    Resource(const std::string& uriPath, bool observable = false) noexcept;
    ~Resource() noexcept;

    /**
     * @brief 获取资源的URI路径
     * 
     * @return URI路径
     */
    std::string getUriPath() const noexcept { return m_uriPath; }

    /**
     * @brief 获取资源是否可观察
     * 
     * @return true 可观察
     * @return false 可观察
     */
    bool isObservable() const noexcept { return m_observable; }

    /**
     * @brief 设置资源的通知信息类型是否为Confirmable
     * 
     * @param enable true为Confirmable，false为Non-confirmable
     * @return 是否设置成功
     *      @retval true 设置成功
     *      @retval false 设置失败，资源未被注册 @see ResourceManager::registerResource(std::unique_ptr<Resource> resource)
     */
    bool enableNotificationMessageConfirmable(bool enable) noexcept;

    /**
     * @brief 对资源的所有观察者发送观察包
     * 
     */
    void notifyObserver() noexcept;

    /**
     * @brief 注册资源的回应接口，当收到请求时，会调用该接口。
     * 
     * @param resourceInterface 资源回应接口
     * @note 一个资源的请求码对应一个回应接口最多可注册四个接口。如果某个请求码没有对应的回应接口，会回应5.01——NotImplemented。
     * @see ResourceInterface
     * 
     */
    void registerInterface(std::unique_ptr<ResourceInterface> resourceInterface) noexcept;

private:
    static void getRequestCallback(coap_resource_t* resource, coap_session_t* session,
        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void putRequestCallback(coap_resource_t* resource, coap_session_t* session,
        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void postRequestCallback(coap_resource_t* resource, coap_session_t* session,
        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void deleteRequestCallback(coap_resource_t* resource, coap_session_t* session,
        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);

    bool initResource() noexcept; // ResourceManager调用
    coap_resource_t* getResource() const noexcept { return m_resource; }  
    void freeResource() noexcept; // ResourceManager调用

    ResourceInterface* getResourceInterface(Information::RequestCode requestCode);
    static void logPdu(coap_pdu_t* request, coap_pdu_t* response) noexcept;

private:
    coap_resource_t* m_resource = nullptr;
    std::string m_uriPath;
    bool m_observable = false;
    bool m_isInit = false;
    std::map<Information::RequestCode, ResourceInterface*> m_resourceInterface;
};


} // namespace CoapPlusPlus