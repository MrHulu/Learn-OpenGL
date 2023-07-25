#include <QtTest>
#include <QSignalSpy>
#include <QDebug>
#include <QByteArray>
#include <QString>

#include <coap3/coap.h>
#include "coap/ContextServer.h"
#include "coap/ResourceManager.h"
#include "coap/Resource.h"
#include "coap/exception.h"
#include "coap/ResourceInterface.h"

using namespace CoapPlusPlus;

class tst_ServerResource : public QObject
{
    Q_OBJECT
public:
    tst_ServerResource()
    {
        m_context = coap_new_context(nullptr);
        QVERIFY(m_context);
        coap_address_t address;
        address.addr.sin.sin_family = AF_INET;
        address.addr.sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.addr.sin.sin_port = htons(_port);
        coap_startup();
        m_session = coap_new_client_session(m_context, nullptr, &address, COAP_PROTO_UDP);
        coap_set_log_level(LOG_DEBUG);
    }
    ~tst_ServerResource()
    {
        coap_session_release(m_session);
        coap_free_context(m_context);
        //coap_cleanup(); _server中已调用
    }

private:
    ContextServer _server;
    coap_context_t *m_context = nullptr;
    coap_session_t *m_session = nullptr;
    int _port = 5683;

private slots:
    void test_ContextServer();
    void test_ResourceRegister(); // 测试资源的注册和注销
    void test_Resource(); // 测试资源的基本接口
    //void test_ResourceInterface(); // todo: 等实现了class Session再测试资源回应接口
};

void tst_ServerResource::test_ContextServer()
{
    // 测试ContextServer的EndPoint相关接口
    QVERIFY_EXCEPTION_THROWN(_server.ioProcess(), DataNotReadyException);
    QVERIFY2(!_server.removeEndPoint(1234), "移除不存在的端点, 预期返回false，实际返回true");
    QVERIFY2(_server.addEndPoint(1234), "添加端点失败");
    QCOMPARE(_server.getEndPointCount(), 1);
    QVERIFY2(_server.removeEndPoint(1234), "移除端点失败");
    QCOMPARE(_server.getEndPointCount(), 0);

    QVERIFY(_server.addEndPoint(_port));
    QVERIFY2(_server.ioProcess() >= 0, "数据已准备好，预期可以开始IO处理返回true，实际返回false");
}

void tst_ServerResource::test_ResourceRegister()
{
    const char *uri1 = "coap://[::1]:40288/coapcpp/test/resource?isObs=true";
    const char *uri2 = "coap://[::1]:40288/coapcpp/test/resource?isObs=false";
    auto& manager = _server.getResourceManager();
    QVERIFY2(!manager.unregisterResource(uri1), "资源还没注册，预期返回false，实际返回true");
    QVERIFY2(manager.registerResource(std::make_unique<Resource>(uri1, true)), "无法注册resource_obs_on");
    QVERIFY2(manager.registerResource(std::make_unique<Resource>(uri2, false)), "无法注册resource_obs_off");
    QVERIFY(manager.unregisterResource(uri1));
    QVERIFY(manager.unregisterResource(uri2));
}

void tst_ServerResource::test_Resource()
{
    const char *uri1 = "coap://[::1]:40288/coapcpp/test/resource?isObs=true";
    const char *uri2 = "coap://[::1]:40288/coapcpp/test/resource?isObs=false";
    Resource resource_obs_on(uri1, true);
    Resource resource_obs_off(uri2, false);
    QCOMPARE(resource_obs_on.getUriPath(), std::string(uri1));
    QCOMPARE(resource_obs_off.getUriPath(), std::string(uri2));
    QCOMPARE(resource_obs_on.isObservable(), true);
    QCOMPARE(resource_obs_off.isObservable(), false);
    QVERIFY2(!resource_obs_on.enableNotificationMessageConfirmable(true), "资源还没注册，预期返回false，实际返回true");
    QVERIFY2(!resource_obs_off.enableNotificationMessageConfirmable(true), "资源还没注册，预期返回false，实际返回true");
}

QTEST_MAIN(tst_ServerResource)

#include "tst_ServerResource.moc"
