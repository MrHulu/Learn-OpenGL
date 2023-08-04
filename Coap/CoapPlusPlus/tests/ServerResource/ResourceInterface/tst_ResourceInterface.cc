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
#include "TestResourceInterface.h"

using namespace CoapPlusPlus;

class tst_ResourceInterface : public QObject
{
    Q_OBJECT
public:
    tst_ResourceInterface()
    {
        // 初始化客户端
        m_context = coap_new_context(nullptr);
        QVERIFY(m_context);
        coap_address_t address;
        coap_address_init(&address);
        address.addr.sin.sin_family = AF_INET;
        address.addr.sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.addr.sin.sin_port = htons(_port);
        coap_startup();
        m_session = coap_new_client_session(m_context, nullptr, &address, COAP_PROTO_UDP);

        // 初始化服务端
        _server.addEndPoint(_port);
        _manager = &_server.getResourceManager();

        coap_set_log_level(LOG_DEBUG);
    }
    ~tst_ResourceInterface()
    {
        coap_session_release(m_session);
        coap_free_context(m_context);
        //coap_cleanup(); _server中已调用
    }

private:
    ContextServer _server;
    ResourceManager* _manager = nullptr;

private:
    coap_context_t *m_context = nullptr;
    coap_session_t *m_session = nullptr;

private:    
    int _port = 5683;

private slots:
    void test_Interface();

};

QTEST_MAIN(tst_ResourceInterface)

#include "tst_ResourceInterface.moc"

void tst_ResourceInterface::test_Interface()
{
    TestResourceInterfaceData data(1);
    auto resource = std::make_unique<Resource>("coapcpp/test/resource", true);
    resource->registerInterface(std::make_unique<TestResourceInterface>(&data, Information::RequestCode::Get));
    QVERIFY(_manager->registerResource(std::move(resource)));
    
    // 发送 GET 请求
    coap_pdu_t *request = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, m_session);
    
    // token
    size_t size;
    uint8_t tokenData[8];
    coap_session_new_token(m_session, &size, tokenData);
    coap_add_token(request, size, tokenData);
    
    // url
    std::string path = "/coapcpp/test/resource";
    coap_uri_t uri;
    uint8_t buf[1024];
    coap_optlist_t *optList = nullptr;
    QVERIFY( coap_split_uri((uint8_t *)path.c_str(), path.size(), &uri) == 0);
    QVERIFY( coap_uri_into_options(&uri, &optList, 1, buf, sizeof(buf)) == 0 );
    QVERIFY( coap_add_optlist_pdu(request, &optList) == 1 );
    coap_send(m_session, request);

    // 等待接收响应
    //coap_io_process(m_context, 100);
    while(_server.isioPending());

    // 检查响应
    QCOMPARE(data.number(), 2);
}