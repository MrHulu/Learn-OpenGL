/**
 * @file ContextServer.h
 * @author Hulu
 * @brief 服务器端Context类定义
 * @version 0.2
 * @date 2023-08-7
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once 

#include "Context.h"
#include "coap/Information/GeneralInformation.h"
#include <map>

struct coap_endpoint_t;
namespace CoapPlusPlus
{

class ResourceManager;
class EndPoint;

class ContextServer : public Context
{
    friend class ResourceManager;
public:
    /**
     * @brief 构造一个管理服务器相关信息的Context对象
     * @details coap_context_t的C++封装，一个Context对应一个服务器或者一个客户端。
     *          CoAP协议栈的全局状态被存储在coap_context_t对象中。 
     * 
     * @exception InternalException 创建Context失败会抛出该异常
     */
    ContextServer();
    ~ContextServer() noexcept;

    /**
     * @brief 启动Persist跟踪，默认不启动
     * @details 当服务器意外重启时，状态信息通常不会在重启过程中持续存在。
     *          通过启用持续支持，可以在重启后恢复状态信息。持续跟踪的信息会重新添加到服务器逻辑中。
     *          持续信息包括动态创建的资源，正在进行的观察订阅，并维护OSCORE保护。
     * @param isEnable 是否启动
     * @return 是否设置成功
     * 
     * @attention 持续支持仅适用于UDP会话
     * @see  https://libcoap.net/doc/reference/develop/man_coap_persist_startup.html
     */
    bool enablePersist(bool isEnable) noexcept;

    /**
     * @brief 得到与客户端会话超时时间，当客户端超过该时间没有发送消息时，会话会被关闭
     * 
     * @return int 超时时间，单位为秒，默认为300秒
     */
    int getSessionCloseTimeout() const noexcept;

    /**
     * @brief 设置与客户端会话超时时间，当客户端超过该时间没有发送消息时，会话会被关闭
     * 
     * @param seconds 
     */
    void setSessionCloseTimeout(int seconds) noexcept;

    /**
     * @brief 为服务器Context添加一个端点，用于与对等体进行通信。
     * 
     * @param port 该端点使用的端口号
     * @param pro 该端点使用的协议，默认为UDP
     * 
     * @return 是否添加成功
     *      @retval false 已经存在该端口号的端点或者内部错误
     *      @retval true 添加成功
     */
    bool addEndPoint(uint16_t port, Information::Protocol pro = Information::Udp) noexcept;

    /**
     * @brief 为服务器Context移除一个端点，用于与对等体进行通信。
     * 
     * @param port 要移除的端点使用的端口号
     * 
     * @return 是否移除成功，如果不存在该端口号的端点则移除失败
     */
    bool removeEndPoint(uint16_t port) noexcept;

    /**
     * @brief 得到一个端点对象
     * 
     * @param port 端点使用的端口号
     * @return EndPoint* 端点对象指针
     * 
     * @exception TargetNotFoundException 未找到对应的端点会抛出该异常
     */
    EndPoint& getEndPoint(uint16_t port) const;

    /**
     * @brief 得到当前Context中的所有端点数量
     * 
     * @return 端点数量
     */
    size_t getEndPointCount() const noexcept { return m_endpoints.size(); }

    /**
     * @brief 得到当前Context中的资源管理器对象
     *        该对象用于管理服务器端的资源，包括创建、删除、查找等操作。
     * 
     * @return 资源管理器对象引用
     * 
     */
    ResourceManager& getResourceManager() const noexcept { return *m_resourceManager; }

private:
    coap_endpoint_t* createEndPoint(uint16_t port, Information::Protocol pro);
    bool isReady() const noexcept override;

private:
    bool m_persistEnable = false;
    std::map<uint16_t, EndPoint*> m_endpoints;
    ResourceManager* m_resourceManager = nullptr;
};


};// namespace CoapPlusPlus