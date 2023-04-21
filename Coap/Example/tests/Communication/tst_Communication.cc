#include <QtTest>
#include <QSignalSpy>
#include <QDebug>

#include "Resource.h"
#include "server.h"
#include "client.h"
#include "utils.h"

using namespace Coap;

class tst_Communication : public QObject {
    Q_OBJECT
public:
    tst_Communication() { }
private:
    SimpleServer* server = nullptr;
    SimpleClient* client = nullptr;
private slots:
    void create_test(); // 创建测试

    void server_simple_receive_test(); // 服务端默认接收测试

    void client_send_receive_test(); // 客户端发送接收测试

    void constom_resource_test(); // 自定义资源测试

    void delete_test(); // 删除测试
};

void tst_Communication::create_test()
{
    server = new SimpleServer();
    client = new SimpleClient();
    QVERIFY(server);
    QVERIFY(client);
    client->registerDefaultResponse();
}

void tst_Communication::server_simple_receive_test() 
{
    // std::string textUrl = "server_simple_receive_test";
    // server->addTextResource(textUrl);
    // server->start(500);
    
    // /** 简单发送一个请求 **/ 
    // auto session = client->getSession();
    // // @todo pdu应该封装一下
    // auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, session);
    // QVERIFY(pdu);
    // coap_add_option(pdu, COAP_OPTION_URI_PATH, textUrl.length(), (const uint8_t*)textUrl.c_str());
    // auto mid = coap_send(session, pdu);
    // QVERIFY(mid != COAP_INVALID_MID);
    
    // QTest::qWait(500);
    // server->stop();
}

void tst_Communication::client_send_receive_test()
{
    std::string textUrl = "client_send_receive_test";
    std::string textUrl1 = "ygjaoihouahiajajsjgkasgasg";
    server->addTextResource(textUrl);
    server->start(30);
    client->asynchronousStartCoapProcess(30);
    
    /** 使用client 提供的接口发送请求 **/ 
    // @todo pdu应该封装一下
    auto session = client->getSession();
    
    // 正确发送
    auto pdu =  coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, session);
    QVERIFY(pdu);
    coap_add_option(pdu, COAP_OPTION_URI_PATH, textUrl.length(), (const uint8_t*)textUrl.c_str());
    bool found_flag = false;
    SimpleClient::CallbackFunc foundCallback = [&found_flag](const coap_pdu_t *send, const coap_pdu_t *receive) {
        auto code = coap_pdu_get_code(receive);
        auto status = getResponseStatus(code);
        found_flag = (status == COAP_RESPONSE_CONTENT);
    };

    // 错误发送
    auto pdu1 = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, session);
    QVERIFY(pdu1);
    coap_add_option(pdu1, COAP_OPTION_URI_PATH, textUrl1.length(), (const uint8_t*)textUrl1.c_str()); // @todo 使用了错误的长度也能回调成功
    bool noFound_flag = false;
    SimpleClient::CallbackFunc noFoundCallback = [&noFound_flag](const coap_pdu_t *send, const coap_pdu_t *receive) {
        auto code = coap_pdu_get_code(receive);
        auto status = getResponseStatus(code);
        noFound_flag = (status == COAP_RESPONSE_NOT_FOUND);
    };

    try { 
        client->sendRequest(pdu, foundCallback); 
        client->sendRequest(pdu1, noFoundCallback); 
    } catch(std::exception& e) { 
        QFAIL( msg.toStdString().c_str()); 
    }
    
    QTest::qWait(300);
    QVERIFY(found_flag);
    QVERIFY(noFound_flag);
    client->stopCoapProcess();
    server->stop();
}

void tst_Communication::constom_resource_test()
{
    // auto textUrl = "constom-resource-test";
    // auto resource = std::make_shared<Resource>(textUrl);
    // resource->setGetHandler([](const Request& request, Response& response) {
    //     response.setPayload("MOZA");
    // });
    // server->addResource(resource);
    // server->start(500);
    // client->registerDefaultResponse();
    
    // // 发送请求
    // auto session = client->getSession();
    // auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, session);
    // QVERIFY(pdu);
    // coap_add_option(pdu, COAP_OPTION_URI_PATH, 4, (const uint8_t*)textUrl);
    // auto mid = coap_send(session, pdu);
    // QVERIFY(mid != COAP_INVALID_MID);
    
    // QTest::qWait(500);
    // server->stop();
}

void tst_Communication::delete_test()
{
    delete server;
    delete client;
}

QTEST_MAIN(tst_Communication)

#include "tst_Communication.moc"
