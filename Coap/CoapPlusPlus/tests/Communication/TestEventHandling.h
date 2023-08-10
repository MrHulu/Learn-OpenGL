#pragma once

#include "coap/EventHandling.h"
#include "coap/Session.h"
#include "coap/Log.h"
#include <set>

namespace CoapPlusPlus
{

class ConncetState
{
public:
    ConncetState() {  m_connectedAddress.clear(); }
    ~ConncetState() { m_connectedAddress.clear(); }

    bool isConnect(uint16_t port) const noexcept { return m_connectedAddress.find(port) != m_connectedAddress.end(); }
    const std::set<uint16_t>& getConnectedAddress() const noexcept { return m_connectedAddress; }
    void add(uint16_t port) noexcept { m_connectedAddress.insert(port); }
    void remove(uint16_t port) noexcept { m_connectedAddress.erase(port); }
    void clear() noexcept { m_connectedAddress.clear(); }

private:
    std::set<uint16_t> m_connectedAddress;
};

class TestEventHandling : public EventHandling
{
public:
    TestEventHandling(ConncetState* connectState) 
        : EventHandling(), m_connects(connectState) 
    {
        if(m_connects == nullptr)
            throw std::invalid_argument("connectState is nullptr");
        m_connects->clear();
    }
    ~TestEventHandling() noexcept { }

protected:
    void onDtlsClosed(Session session) noexcept override {}
    void onDtlsConnected(Session session) noexcept override {}
    void onDtlsRenegotiation(Session session) noexcept override {}
    void onDtlsError(Session session) noexcept override {}
    void onTcpClosed(Session session) noexcept override {}
    void onTcpConnected(Session session) noexcept override {}
    void onTcpFailed(Session session) noexcept override {}
    void onSessionClosed(Session session) noexcept override {}
    void onSessionConnected(Session session) noexcept override {}
    void onSessionFailed(Session session) noexcept override {}
    void onPartialBlock(Session session) noexcept override {}
    void onXmitBlockFail(Session session) noexcept override {}
    void onServerSessionConnected(Session session) noexcept override ;
    void onServerSessionClosed(Session session) noexcept override;
    void onBadPacket(Session session) noexcept override {}
    void onMSGRetransmit(Session session) noexcept override {}
    void onKeepAliveFailured(Session session) noexcept override;

private:
    ConncetState* m_connects = nullptr;
};


}  // namespace CoapPlusPlus