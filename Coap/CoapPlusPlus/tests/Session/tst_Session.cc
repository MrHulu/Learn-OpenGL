#include <QtTest>
#include <QSignalSpy>
#include <QDebug>
#include <QByteArray>
#include <QString>

#include "coap/ContextClient.h"
#include "coap/Session.h"
#include "coap/exception.h"
#include "coap/DataStruct/Address.h"

using namespace CoapPlusPlus;

class tst_Session : public QObject
{
    Q_OBJECT
public:
    tst_Session()
    {
        coap_set_log_level(LOG_DEBUG);
    }
    ~tst_Session()
    {
    }

private:
    ContextClient _test_client;
    Session* _test_session = nullptr;
    int _port = 5683;

private slots:
    void test_ContextClient();

    void test_Session();
};

QTEST_MAIN(tst_Session)

#include "tst_Session.moc"


void tst_Session::test_ContextClient()
{
    // 测试ContextClient的Session相关接口
    QVERIFY_EXCEPTION_THROWN(_test_client.ioProcess(), DataNotReadyException);
    QVERIFY2(!_test_client.removeSession(_port, Information::Udp), "移除不存在的会话, 预期返回false，实际返回true");
    QVERIFY2(_test_client.addSession(_port), "添加端点失败");
    QCOMPARE(_test_client.getSessionCount(), 1);
    QVERIFY2(_test_client.removeSession(_port, Information::Udp), "移除端点失败");
    QCOMPARE(_test_client.getSessionCount(), 0);

    QVERIFY(_test_client.addSession(_port));
    QVERIFY2(_test_client.ioProcess() >= 0, "数据已准备好，预期可以开始IO处理返回true，实际返回false");
    QVERIFY_EXCEPTION_THROWN(_test_client.getSession(_port, Information::Dtls), TargetNotFoundException);
    _test_session = _test_client.getSession(_port, Information::Udp);
}

void tst_Session::test_Session()
{
    QCOMPARE(_test_session->getProtocol(), Information::Udp);
    QCOMPARE(_test_session->getSessionState(), Information::Established);
    
    // 测试Session的Address相关接口
    QCOMPARE(_test_session->getRemoteAddress().getPort(), _port);
    QCOMPARE(_test_session->getRemoteAddress().getIpAddress(), std::string("127.0.0.1"));
    QCOMPARE(_test_session->getLocalAddress().getIpAddress(), std::string("127.0.0.1"));
    
    // 测试超时相关接口
    QVERIFY(qFuzzyCompare(_test_session->getAckTimeout(), 2.0f));
    _test_session->setAckTimeout(1.5f);
    QVERIFY(qFuzzyCompare(_test_session->getAckTimeout(), 1.5f));

    // 测试重传次数相关接口
    QCOMPARE(_test_session->getMaxRetransmit(), 4);
    _test_session->setMaxRetransmit(3);
    QCOMPARE(_test_session->getMaxRetransmit(), 3);

    auto context = dynamic_cast<ContextClient*>(const_cast<Context*>(Session::GetContext(_test_session)));
    QVERIFY(context != nullptr);
    QCOMPARE(context, &_test_client);
}
