#include <QtTest>
#include <QSignalSpy>
#include <QDebug>

#include "coap/Context.h"
#include "coap/Utils.h"

using namespace CoapPlusPlus;

class tst_Communication : public QObject {
    Q_OBJECT
public:
    tst_Communication() { }
    ~tst_Communication() {  }
private:
    // Context* server_cxt = nullptr;

    // Context* client_cxt = nullptr;
private slots:
    // void create_test(); // 创建测试

    // void server_simple_receive_test(); // 服务端默认接收测试

    // void client_send_receive_test(); // 客户端发送接收测试

    // void constom_resource_test(); // 自定义资源测试

    // void delete_test(); // 删除测试
};

// void tst_Communication::create_test()
// {
//     server_cxt = new Context();
//     QVERIFY(server_cxt);
//     server_cxt->addEndPoint();
//     server = server_cxt->server();
//     QVERIFY(server);

//     client_cxt = new Context();
//     QVERIFY(client_cxt);
//     client = client_cxt->client();
//     QVERIFY(client);
//     client->registerDefaultResponse();
// }

// void tst_Communication::server_simple_receive_test() 
// {
//     std::string textUrl = "server_simple_receive_test";
//     server->addTextResource(textUrl);
//     server->start(500);
    
//     /** 简单发送一个请求 **/ 
//     auto session = client->getSession();
//     // @todo pdu应该封装一下
//     auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, session);
//     QVERIFY(pdu);
//     coap_add_option(pdu, COAP_OPTION_URI_PATH, textUrl.length(), (const uint8_t*)textUrl.c_str());
//     auto mid = coap_send(session, pdu);
//     QVERIFY(mid != COAP_INVALID_MID);
    
//     QTest::qWait(500);
//     server->stop();
// }

// void tst_Communication::client_send_receive_test()
// {
//     // 添加资源
//     std::string textUrl = "client_send_receive_test";
//     server->addTextResource(textUrl);
//     server->start(30);
//     client->asynchronousStartCoapProcess(30);
    
//     /** 使用client 提供的接口发送请求 **/ 
//     // @todo pdu应该封装一下
//     auto session = client->getSession();
    
//     // 正确发送
//     auto pdu =  coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, session);
//     QVERIFY(pdu);
//     coap_add_option(pdu, COAP_OPTION_URI_PATH, textUrl.length(), (const uint8_t*)textUrl.c_str());
//     bool found_flag = false;
//     SimpleClient::CallbackFunc foundCallback = [&found_flag](const coap_pdu_t *send, const coap_pdu_t *receive) {
//         auto code = coap_pdu_get_code(receive);
//         auto status = getResponseStatus(code);
//         found_flag = (status == COAP_RESPONSE_CONTENT);
//     };

//     // 错误发送
//     auto pdu1 = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, session);
//     QVERIFY(pdu1);
//     std::string textUrl1 = "ygjaoihouahiajajsjgkasgasg";
//     coap_add_option(pdu1, COAP_OPTION_URI_PATH, textUrl1.length(), (const uint8_t*)textUrl1.c_str());
//     bool noFound_flag = false;
//     SimpleClient::CallbackFunc noFoundCallback = [&noFound_flag](const coap_pdu_t *send, const coap_pdu_t *receive) {
//         auto code = coap_pdu_get_code(receive);
//         auto status = getResponseStatus(code);
//         noFound_flag = (status == COAP_RESPONSE_NOT_FOUND);
//     };

//     try { 
//         client->sendRequest(pdu, foundCallback); 
//         client->sendRequest(pdu1, noFoundCallback); 
//     } catch(std::exception& e) { 
//         auto msg = QString("client_send_receive_test-throw: %1").arg(e.what());
//         QFAIL( msg.toStdString().c_str()); 
//     }
    
//     QTest::qWait(300);
//     QVERIFY(found_flag);
//     //QVERIFY(noFound_flag);
//     client->stopCoapProcess();
//     server->stop();
// }

// void tst_Communication::constom_resource_test()
// {
//     std::string textUrl = "constom-resource-test";
//     try {
//         auto resource = std::make_shared<Tst_TextResource>(textUrl);
//         server->addResource(resource);
//     } catch (std::exception& e) { 
//         auto msg = QString("constom_resource_test--throw: %1").arg(e.what());
//         QFAIL( msg.toStdString().c_str()); 
//     }
    
//     server->start(30);
//     client->asynchronousStartCoapProcess(30);
    
//     // 发送请求
//     auto pdu =  coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, client->getSession());
//     QVERIFY(pdu);
//     coap_add_option(pdu, COAP_OPTION_URI_PATH, textUrl.length(), (const uint8_t*)textUrl.c_str());
//     bool flag = false;
//     SimpleClient::CallbackFunc foundCallback = [&flag](const coap_pdu_t *send, const coap_pdu_t *receive) {
//         auto code = coap_pdu_get_code(receive);
//         auto status = getResponseStatus(code);
//         flag = (status == COAP_RESPONSE_CONTENT);
//     };
    
//     try { 
//         client->sendRequest(pdu, foundCallback);
//     } catch(std::exception& e) { 
//         auto msg = QString("constom_resource_test-throw: %1").arg(e.what());
//         QFAIL( msg.toStdString().c_str()); 
//     }

//     QTest::qWait(500);
//     QVERIFY(flag);
//     client->stopCoapProcess();
//     server->stop();
// }

// void tst_Communication::delete_test()
// {
//     delete server_cxt;
//     delete client_cxt; 
// }

QTEST_MAIN(tst_Communication)

#include "tst_Communication.moc"
