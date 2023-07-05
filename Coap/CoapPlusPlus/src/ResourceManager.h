/**
 * @file ResourceManager.h
 * @author Hulu
 * @brief coap_resource_t 的资源管理器类定义
 * @version 0.1
 * @date 2023-06-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include "ContextServer.h"

namespace CoapPlusPlus
{

class Resource;
class ResourceManager
{
public:
    ResourceManager(ContextServer& context);
    ~ResourceManager() noexcept;

    /**
     * @brief 为服务器注册一个资源
     * 
     * @param resource std::unique_ptr<Resource>的
     * @return 是否注册成功
     *      @retval true 注册成功
     *      @retval false 注册失败，该资源已经被注册或者资源指针为空
     * 
     * @note 资源对象将由资源管理器接管，不需要用户手动释放
     * @see unregisterResource(const std::string& uriPath)
     */
    bool registerResource(std::unique_ptr<Resource> resource) noexcept;

    /**
     * @brief 为服务器注销一个资源
     * 
     * @param uriPath 该资源的URI路径，一个资源只能有一个URI路径
     * @return 是否注销成功
     *      @retval true 注销成功
     *      @retval false 注销失败，没有找到该资源
     * 
     * @note 当收到一个delete请求时，需要调用该函数注销资源
     * @see registerResource(std::unique_ptr<Resource> resource)
     */
    bool unregisterResource(const std::string& uriPath) noexcept;

private:
    ContextServer& _context;
    std::map<std::string, Resource*> m_resources;
};


};  // namespace CoapPlusPlus