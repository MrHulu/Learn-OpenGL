#include "TestEventHandling.h"
#include "coap/DataStruct/Address.h"

namespace CoapPlusPlus
{
    
void TestEventHandling::onServerSessionConnected(Session session) noexcept
{
    Log::Logging(LOG_LEVEL::INFO, "TestEventHandling::onServerSessionConnected, port:%d\n", session.getRemoteAddress().getPort());
    m_connects->add(session.getRemoteAddress().getPort());
}

void TestEventHandling::onServerSessionClosed(Session session) noexcept
{
    Log::Logging(LOG_LEVEL::INFO, "TestEventHandling::onServerSessionClosed\n");
    m_connects->remove(session.getRemoteAddress().getPort());
}

void TestEventHandling::onKeepAliveFailured(Session session) noexcept
{
    Log::Logging(LOG_LEVEL::INFO, "TestEventHandling::onKeepAliveFailured\n");
    m_connects->remove(session.getRemoteAddress().getPort());
}

} // end of namespace CoapPlusPlus