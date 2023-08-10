#include <QtTest>
#include <QSignalSpy>
#include <QDebug>

#include "TestEventHandling.h"
#include "coap/ContextServer.h"
#include "coap/ContextClient.h"
#include "coap/DataStruct/Address.h"
#include "coap/Pdu/ResponsePdu.h"
#include "coap/Session.h"
#include <functional>

using namespace CoapPlusPlus;
class tst_Communication : public QObject {
    Q_OBJECT
public:
    tst_Communication() { Log::SetLevel(LOG_LEVEL::DEBUG); }
    ~tst_Communication() {  }
private:
    ContextServer _server;
    ContextClient _client;
    int _port = 5683;

private slots:
    void test_client_connectState();
    void test_server_connectState();
};

QTEST_MAIN(tst_Communication)

#include "tst_Communication.moc"

void tst_Communication::test_client_connectState()
{
    // 服务器建立
    QVERIFY( _server.addEndPoint(_port, Information::Udp) );
    ConncetState state; 
    _server.registerEventHandling(std::make_unique<TestEventHandling>(&state));
    QVERIFY( state.getConnectedAddress().empty() );

    // 客户端连接服务器，开始握手
    QVERIFY( _client.addSession(_port, Information::Udp) );
    auto session = _client.getSession(_port, Information::Udp);
    QVERIFY( session != nullptr );
    _client.setHandshakeInterval(3);
    auto clientAddress = session->getLocalAddress();

    // ...等待握手完成
    QTest::qWait(3000);
    while(1) {
        auto client_result = _client.isioPending();
        auto server_result = _server.isioPending();
        if(!client_result && !server_result)
            break;
    }
    QVERIFY( state.isConnect(clientAddress.getPort()) );

    // 客户端主动断开连接
    _server.setSessionCloseTimeout(5);
    QVERIFY( _client.removeSession(_port, Information::Udp) );

    // ...等待会话超时
    QTest::qWait(5000);
    while(1) {
        auto client_result = _client.isioPending();
        auto server_result = _server.isioPending();
        if(!client_result && !server_result)
            break;
    }
    QVERIFY( !state.isConnect(clientAddress.getPort()) );
    QCOMPARE( state.getConnectedAddress().size(), 0 );
    QVERIFY( _server.removeEndPoint(_port) );
    _server.registerEventHandling(nullptr);
}

void tst_Communication::test_server_connectState()
{
    ConncetState state; 
    _client.registerEventHandling(std::make_unique<TestEventHandling>(&state));
    _client.registerHandshakeResponedFunction([&state](const CoapPlusPlus::Session* session, 
                                                const CoapPlusPlus::ResponsePdu* response, int id) 
    {
        if(session) {
            state.add(session->getRemoteAddress().getPort());
        }
    });

    // 客户端连接服务器，开始握手
    QVERIFY( _client.addSession(_port, Information::Udp) );
    auto session = _client.getSession(_port, Information::Udp);
    QVERIFY( session != nullptr );
    _client.setHandshakeInterval(3);
    QCOMPARE( state.getConnectedAddress().size(), 0 );

    // 服务器建立
    QVERIFY( _server.addEndPoint(_port, Information::Udp) );
    QTest::qWait(3000);
    while(1) {
        auto client_result = _client.isioPending();
        auto server_result = _server.isioPending();
        if(!client_result && !server_result)
            break;
    }
    QCOMPARE( state.getConnectedAddress().size(), 1 );
    QVERIFY( state.isConnect(session->getRemoteAddress().getPort()) );
}
