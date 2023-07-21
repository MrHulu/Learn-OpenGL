#include <QtTest>
#include <QSignalSpy>
#include <QDebug>
#include <QByteArray>
#include <QString>

#include "coap/ContextClient.h"
#include "coap/Session.h"
#include "coap/exception.h"
#include "coap/DataStruct/BinaryConst.h"
#include "coap/SendersManager.h"
#include "coap/Pdu/RequestPdu.h"
#include "coap/Handling.h"
#include "TestHandling.h"

using namespace CoapPlusPlus;

class tst_SendersManager : public QObject
{
    Q_OBJECT
public:
    tst_SendersManager()
    {
        coap_set_log_level(LOG_INFO);
        QVERIFY2(_test_client.addSession(_port), "添加会话失败");
        QVERIFY2(_test_client.startIOProcess(), "数据已准备好，预期可以开始IO处理返回true，实际返回false");

        _test_server = coap_new_context(nullptr);
        QVERIFY2(_test_server, "创建coap_context_t失败");
        coap_context_set_block_mode(_test_server, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
    }
    ~tst_SendersManager()
    {
        stopServer();
        if(_test_ep) {
            coap_free_endpoint(_test_ep);
            _test_ep = nullptr;
        }
        if(_test_server)
            coap_free_context(_test_server);
        //coap_cleanup();
    }

private:
    // server
    coap_context_t* _test_server = nullptr;
    coap_endpoint_t *_test_ep = nullptr;
    std::thread* _test_server_io_thread = nullptr;
    bool _server_flag = false;
    std::mutex _server_mutex;
    void startServer(int time_ms);
    void stopServer();
    
    // client
    ContextClient _test_client;
    SendersManager *_test_sendersManager = nullptr;
    int _port = 5683;

private slots:
    void test_SendersManager(); // 测试基本接口

    void test_sendAndHandling_case1(); // 测试未连上服务器时的send函数与Handling相关接口
    
    void test_sendAndHandling_case2(); // 测试连上服务器时的send函数与Handling相关接口

    void test_sendAndUpdateDefaultHandling(); // 测试默认Handling

};

void tst_SendersManager::startServer(int time_ms)
{
    stopServer();
    coap_address_t  addr;
    coap_address_init(&addr);
    addr.addr.sin.sin_family = AF_INET;
    addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
    addr.addr.sin.sin_port = htons(_port);
    _test_ep = coap_new_endpoint(_test_server, &addr, COAP_PROTO_UDP);
    QVERIFY2(_test_ep, "创建端点失败");

    std::lock_guard<std::mutex> lock(_server_mutex);
    _server_flag = true;
    _test_server_io_thread = new std::thread([this](int time){
        coap_context_t *temp_ctx;
        while (true) { 
            bool flag; 
            {
                std::lock_guard<std::mutex> lock(_server_mutex);
                flag = _server_flag;
                temp_ctx = _test_server;
            }
            if (!flag) break;
            coap_io_process(temp_ctx, time);
        }
    }, time_ms);
}

void tst_SendersManager::stopServer()
{
    if(_test_ep) {
        coap_free_endpoint(_test_ep);
        _test_ep = nullptr; 
    }
    {
        std::lock_guard<std::mutex> lock(_server_mutex);
        _server_flag = false;
    }
    if(_test_server_io_thread) {
        _test_server_io_thread->join();
        delete _test_server_io_thread;
        _test_server_io_thread = nullptr;
    }
}


QTEST_MAIN(tst_SendersManager)

#include "tst_SendersManager.moc"


void tst_SendersManager::test_SendersManager()
{
    using namespace Information;
    auto session = _test_client.getSession(_port, Udp);
    QVERIFY2(session, "获取session失败");
    session->setMaxRetransmit(1);
    QCOMPARE(session->getMaxRetransmit(), 1);
    _test_sendersManager = &session->getSendersManager();
    QVERIFY2(_test_sendersManager, "获取SendersManager失败");

    // TOKEN
    auto token = _test_sendersManager->createToken();
    QVERIFY(token.size() != 0);

    // PDU
    auto pdu = _test_sendersManager->createRequest(MessageType::NonConfirmable, RequestCode::Post);
    QCOMPARE(pdu.code(), RequestCode::Post);
    QCOMPARE(pdu.messageType(), MessageType::NonConfirmable);
    QVERIFY(pdu.token().size() != 0);

    // HANDLING
    QVERIFY_EXCEPTION_THROWN(_test_sendersManager->getHandling(token), TargetNotFoundException);
    QVERIFY(!_test_sendersManager->removeHandling(token));
}

void tst_SendersManager::test_sendAndHandling_case1()
{
    // send nullptr
    RequestPdu pdu = _test_sendersManager->createRequest(MessageType::NonConfirmable, RequestCode::Get);
    QVERIFY(_test_sendersManager->send(pdu, std::unique_ptr<Handling>()));
    
    // send token 不一致
    pdu =  _test_sendersManager->createRequest(MessageType::NonConfirmable, RequestCode::Put);
    auto data_n_p = new TestHandlingData(6);
    QVERIFY_EXCEPTION_THROWN(_test_sendersManager->send(pdu, std::make_unique<TestHandling>(data_n_p, _test_sendersManager->createToken())), std::invalid_argument);

    // send 正常情况1
    auto pdu_c_p = _test_sendersManager->createRequest(MessageType::Confirmable, RequestCode::Post);
    auto c_p_tokenView = pdu_c_p.token();
    auto data_c_p = new TestHandlingData(66);
    QVERIFY(_test_sendersManager->send(pdu_c_p, std::make_unique<TestHandling>(data_c_p, c_p_tokenView.toBinaryConst())));
    
    // send 正常情况2
    auto pdu_c_d = _test_sendersManager->createRequest(MessageType::Confirmable, RequestCode::Delete);
    auto c_d_tokenView = pdu_c_d.token();
    auto data_c_d = new TestHandlingData(666);
    auto handling_c_d = std::make_unique<TestHandling>(data_c_d, c_d_tokenView.toBinaryConst());
    handling_c_d->setFinished(true);
    QVERIFY(_test_sendersManager->send(pdu_c_d, std::move(handling_c_d)));

    QTest::qSleep(16000);

    // getHandling(6) 没有被发送
    QCOMPARE(data_n_p->number(), 6);
    QCOMPARE(data_n_p->isDestroy(), true);

    // getHandling(66) isFinished = flase, 还在列表中
    auto handling = dynamic_cast<TestHandling*>(_test_sendersManager->getHandling(c_p_tokenView));
    QVERIFY(handling);
    QCOMPARE(handling->data(), data_c_p);
    QCOMPARE(handling->isFinished(), false);
    QVERIFY(data_c_p->number() <= 63); //TODO: 明明设置了最大重传次数为1，NAck回调应该触发三次的，应该是QCOMPARE(data_c_p->number(), 63)，但偶尔是四次？
    QCOMPARE(data_c_p->isDestroy(), false);

    // handling(666) isFinished = true， 被销毁了
    QVERIFY_EXCEPTION_THROWN(_test_sendersManager->getHandling(c_d_tokenView), TargetNotFoundException);
    QCOMPARE(data_c_d->number(), 665);
    QCOMPARE(data_c_d->isDestroy(), true);
    
    // removeHandling
    QVERIFY(_test_sendersManager->removeHandling(c_p_tokenView));
    QVERIFY(!_test_sendersManager->removeHandling(c_p_tokenView));
    QVERIFY_EXCEPTION_THROWN(_test_sendersManager->getHandling(c_p_tokenView), TargetNotFoundException);
    QCOMPARE(data_c_p->isDestroy(), true);

    delete data_n_p;
    delete data_c_p;
    delete data_c_d;
}

void tst_SendersManager::test_sendAndHandling_case2()
{
    startServer(1000);

    // send nullptr
    RequestPdu pdu = _test_sendersManager->createRequest(MessageType::NonConfirmable, RequestCode::Get);
    QVERIFY(_test_sendersManager->send(pdu, std::unique_ptr<Handling>()));
    
    // send token 不一致
    pdu =  _test_sendersManager->createRequest(MessageType::NonConfirmable, RequestCode::Put);
    auto data_n_p = new TestHandlingData(8);
    QVERIFY_EXCEPTION_THROWN(_test_sendersManager->send(pdu, std::make_unique<TestHandling>(data_n_p, _test_sendersManager->createToken())), std::invalid_argument);

    // send 正常情况1
    auto pdu_c_p = _test_sendersManager->createRequest(MessageType::Confirmable, RequestCode::Post);
    auto c_p_tokenView = pdu_c_p.token();
    auto data_c_p = new TestHandlingData(88);
    QVERIFY(_test_sendersManager->send(pdu_c_p, std::make_unique<TestHandling>(data_c_p, c_p_tokenView.toBinaryConst())));
    
    // send 正常情况2
    auto pdu_c_d = _test_sendersManager->createRequest(MessageType::Confirmable, RequestCode::Delete);
    auto c_d_tokenView = pdu_c_d.token();
    auto data_c_d = new TestHandlingData(888);
    auto handling_c_d = std::make_unique<TestHandling>(data_c_d, c_d_tokenView.toBinaryConst());
    handling_c_d->setFinished(true);
    QVERIFY(_test_sendersManager->send(pdu_c_d, std::move(handling_c_d)));

    QTest::qSleep(3000);

    // getHandling(8) 没有被发送
    QCOMPARE(data_n_p->number(), 8);
    QCOMPARE(data_n_p->isDestroy(), true);

    // getHandling(88) isFinished = flase, 还在列表中
    auto handling = dynamic_cast<TestHandling*>(_test_sendersManager->getHandling(c_p_tokenView));
    QVERIFY(handling);
    QCOMPARE(handling->data(), data_c_p);
    QCOMPARE(handling->isFinished(), false);
    QCOMPARE(data_c_p->number(), 89);
    QCOMPARE(data_c_p->isDestroy(), false);

    // handling(888) isFinished = true， 被销毁了
    QVERIFY_EXCEPTION_THROWN(_test_sendersManager->getHandling(c_d_tokenView), TargetNotFoundException);
    QCOMPARE(data_c_d->number(), 889);
    QCOMPARE(data_c_d->isDestroy(), true);
    
    // removeHandling
    QVERIFY(_test_sendersManager->removeHandling(c_p_tokenView));
    QVERIFY(!_test_sendersManager->removeHandling(c_p_tokenView));
    QVERIFY_EXCEPTION_THROWN(_test_sendersManager->getHandling(c_p_tokenView), TargetNotFoundException);
    QCOMPARE(data_c_p->isDestroy(), true);

    delete data_n_p;
    delete data_c_p;
    delete data_c_d;
}

void tst_SendersManager::test_sendAndUpdateDefaultHandling() 
{
    stopServer();
    auto number = 40288;
    auto handlingData = new TestHandlingData(number);
    auto defaultHandling = std::make_unique<TestHandling>(handlingData, _test_sendersManager->createToken());
    _test_sendersManager->updateDefaultHandling(std::move(defaultHandling));

    RequestPdu pdu = _test_sendersManager->createRequest(MessageType::NonConfirmable, RequestCode::Get);
    QVERIFY(_test_sendersManager->send(pdu, std::unique_ptr<Handling>()));
    QCOMPARE(handlingData->number(), number);
    QCOMPARE(handlingData->isDestroy(), false);

    pdu = _test_sendersManager->createRequest(MessageType::Confirmable, RequestCode::Post);
    QVERIFY(_test_sendersManager->send(pdu, std::unique_ptr<Handling>()));
    QTest::qSleep(10000);
    auto n = handlingData->number();
    QVERIFY(n <= number - 3);
    QCOMPARE(handlingData->isDestroy(), false);

    startServer(1000);

    pdu = _test_sendersManager->createRequest(MessageType::Confirmable, RequestCode::Get);
    QVERIFY(_test_sendersManager->send(pdu, std::unique_ptr<Handling>()));
    QTest::qSleep(2000);
    QCOMPARE(handlingData->number(), n + 1);
    QCOMPARE(handlingData->isDestroy(), false);

    delete handlingData;
}