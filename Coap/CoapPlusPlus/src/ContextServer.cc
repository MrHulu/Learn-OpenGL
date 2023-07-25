/**
 * @file ContextServer.cc
 * @author Hulu
 * @brief 服务器端Context类实现
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <coap3/coap.h>
#include "ContextServer.h"
#include "ResourceManager.h"
#include "EndPoint.h"
#include "coap/exception.h"

namespace CoapPlusPlus {

ContextServer::ContextServer() : Context() {
    m_resourceManager = new ResourceManager(*this);
}

ContextServer::~ContextServer() noexcept {
    while(isBusy());///必须先停止IO进程，否则会导致资源已经被释放，但是IO进程的回调还在使用资源
    delete m_resourceManager;
    m_resourceManager = nullptr;
    
    if(m_persistEnable == true) {
        enablePersist(false);
    }

    // 删除所有端点对象
    for (auto& pair : m_endpoints) {
        delete pair.second;
    }
    m_endpoints.clear();
}

bool ContextServer::enablePersist(bool isEnable) noexcept {
    if(isEnable == true) {
        m_persistEnable = coap_persist_startup(getContext(), 
                            "/tmp/coap_dyn_resource_save_file",
                            "/tmp/coap_observe_save_file",
                            "/tmp/coap_obs_cnt_save_file", 10);
        return m_persistEnable;
    }else{
        m_persistEnable = isEnable;
        coap_persist_stop(getContext());
    }
    return true;
}

bool ContextServer::addEndPoint(uint16_t port, Information::Protocol pro) noexcept {
    // 检查端点是否已存在
    if (m_endpoints.find(port) != m_endpoints.end()) {
        return false; 
    }
    // 创建端点
    try{
        coap_endpoint_t* endpoint = createEndPoint(port, pro);
        // 创建并存储端点对象
        EndPoint* endPointObj = new EndPoint(endpoint);
        m_endpoints.emplace(port, endPointObj);
        return true;
    }catch(const InternalException& e) {
        coap_log_warn("Create endpoint failed: %s\n", e.what());
        return false;
    }
}

bool ContextServer::removeEndPoint(uint16_t port) noexcept {
    // 检查端点是否存在
    auto it = m_endpoints.find(port);
    if (it == m_endpoints.end()) {
        return false;
    }
    
    // 删除并释放端点对象
    delete it->second;
    m_endpoints.erase(it);
    return true;
}

EndPoint& ContextServer::getEndPoint(uint16_t port) const {
    // 检查端点是否存在
    auto it = m_endpoints.find(port);
    if (it == m_endpoints.end()) {
        throw TargetNotFoundException("Endpoint with port " + std::to_string(port) + " does not exist");
    }
    
    // 返回端点对象引用
    return *(it->second);
}

coap_endpoint_t* ContextServer::createEndPoint(uint16_t port, Information::Protocol pro) {
    coap_address_t  addr;
    coap_address_init(&addr);

    // 设置地址族和端口
    addr.addr.sin.sin_family = AF_INET;
    // 如果添加这一行代码，服务器将监听所有网络接口，这意味着它可以接收来自任何IP地址的客户端连接。
    // 如果不添加这一行代码，服务器将只监听默认的网络接口（通常是localhost或127.0.0.1）。这意味着服务器只能处理来自本地计算机的客户端请求。
    addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
    addr.addr.sin.sin_port = htons(port);
    
    // 创建端点
    auto ep = coap_new_endpoint(m_ctx, &addr, (coap_proto_t)pro);
    if (ep == nullptr) {
        throw InternalException("Failed to create CoAP endpoint");
    }
    return ep;
}

bool ContextServer::isReady() const noexcept
{
    return m_endpoints.size() > 0;
}

} // namespace CoapPlusPlus
