#include <coap3/coap.h>
#include "ContextClient.h"
#include "coap/Session.h"
#include "coap/exception.h"

namespace CoapPlusPlus
{

ContextClient::ContextClient() : Context()
{
}

ContextClient::~ContextClient() noexcept
{
    while(isBusy());///必须先停止IO进程，否则会导致资源已经被释放，但是IO进程的回调还在使用资源
    // 删除所有会话对象
    for (auto& pair : m_sessions) {
        delete pair.second;
    }
    m_sessions.clear();
}

bool ContextClient::addSession(uint16_t port, Information::Protocol pro) noexcept
{
    // 检查会话是否已存在
    if (m_sessions.find({ port, pro }) != m_sessions.end()) {
        return false; 
    }
    // 创建会话
    try{
        auto raw_session = createSession(port, pro);
        // 创建并存储会话对象
        auto session = new Session(raw_session);
        m_sessions.emplace(std::make_pair(port, pro), session);
        return true;
    }catch(const InternalException& e) {
        coap_log_warn("Create session failed: %s\n", e.what());
        return false;
    }
}

bool ContextClient::removeSession(uint16_t port, Information::Protocol pro) noexcept
{
    // 检查会话是否存在
    auto it = m_sessions.find({ port, pro });
    if (it == m_sessions.end()) {
        return false;
    }
    
    // 删除并释放会话对象
    delete it->second;
    m_sessions.erase(it);
    return true;
}

Session* ContextClient::getSession(uint16_t port, Information::Protocol pro) const
{
    // 检查会话是否存在
    auto it = m_sessions.find({ port, pro });
    if (it == m_sessions.end()) {
        throw TargetNotFoundException("Session with port " + std::to_string(port) 
                                    + "and protocol" + std::to_string(pro) + " does not exist");
    }
    
    return it->second;
}

bool ContextClient::isReady() const noexcept
{
    return m_sessions.size() > 0;
}

coap_session_t *ContextClient::createSession(uint16_t port, Information::Protocol pro)
{
    coap_address_t  addr;
    coap_address_init(&addr);

    // 设置地址族和端口
    addr.addr.sin.sin_family = AF_INET;
    // 只与本地通信
    addr.addr.sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.addr.sin.sin_port = htons(port);
    
    // 创建会话
    auto raw_session = coap_new_client_session(m_ctx, nullptr, &addr, (coap_proto_t)pro);
    if (raw_session == nullptr) {
        throw InternalException("Failed to create CoAP session");
    }
    return raw_session;
}

} // namespace CoapPlusPlus