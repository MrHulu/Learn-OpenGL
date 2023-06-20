#include <QtTest>
#include <QSignalSpy>
#include <QDebug>
#include <QByteArray>
#include <QString>

#include "coap/Pdu/Encoder.h"
#include "coap/Pdu/Decoder.h"
#include "coap/Pdu/Option.h"
#include "coap/Pdu/Options.h"
#include "coap/Pdu/OptFilter.h"

#include "coap/Pdu/ResponsePdu.h"
#include "coap/Pdu/RequestPdu.h"

using namespace CoapPlusPlus;

class tst_Pdu : public QObject
{
    Q_OBJECT
public:
    tst_Pdu() {
        m_context = coap_new_context(nullptr);
        coap_set_log_level(LOG_INFO);
        QVERIFY(m_context);
        coap_address_t address;
        address.addr.sin.sin_family = AF_INET;
        address.addr.sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.addr.sin.sin_port = htons(5683);
        coap_startup();
        m_session = coap_new_client_session(m_context, nullptr, &address, COAP_PROTO_UDP);
    }
    ~tst_Pdu() {
        coap_session_release(m_session);
        coap_free_context(m_context);
        coap_cleanup();
    }

private:
    coap_context_t* m_context = nullptr;
    coap_session_t* m_session = nullptr;

private slots:
    void test_Option(); // 测试Option类
    void test_Options(); // 测试Options类
    void test_Options1(); // 测试Options多次添加
    void test_Options2(); // 测试添加了payload之后再添加Option
    void test_OptFilter(); // 测试OptFilter类
    void test_Encoder_Decoder(); // 测试Encoder和Decoder类

    void test_token(); // 测试token
    void test_requestPdu(); // 测试RequestPdu类
    void test_responsePdu(); // 测试ResponsePdu类
};

void tst_Pdu::test_Option()
{
    const char* uri = "coap://[::1]:40288/coapcpp/test/pdu/option";
    uint8_t buf[1024];
    uint8_t* sbuf = buf;
    size_t buflen;
    int res;
    coap_optlist_t *optlist_chain = nullptr;
    
    /*** 模拟发送准备 ***/ 
    auto pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, 0, 1024);
    /* Add in the URI options */
    buflen = sizeof(buf);
    res = coap_split_path((const uint8_t*)uri, strlen(uri), sbuf, &buflen);
    QCOMPARE(res, 7);
    while(res--) {
        qDebug() << "path:" << QString::fromUtf8((const char*)coap_opt_value(sbuf), coap_opt_length(sbuf));
        QCOMPARE(coap_insert_optlist(&optlist_chain, coap_new_optlist(COAP_OPTION_URI_PATH, coap_opt_length(sbuf), coap_opt_value(sbuf))), 1);
        sbuf += coap_opt_size(sbuf);
    } 
    /* Add Obs */ 
    QCOMPARE(coap_insert_optlist(&optlist_chain, coap_new_optlist(COAP_OPTION_OBSERVE, coap_encode_var_safe(buf, sizeof(buf), COAP_OBSERVE_ESTABLISH),buf)), 1);
    QVERIFY(coap_add_optlist_pdu(pdu, &optlist_chain));
    coap_delete_optlist(optlist_chain);

    /*** 模拟接收 ***/
    /* 测试Option的getValue() 和 getLength()函数 */
    coap_opt_iterator_t opt_iter;
    auto opt = coap_check_option(pdu, COAP_OPTION_OBSERVE, &opt_iter);
    Option option((Information::OptionNumber)opt_iter.number, opt);
    QCOMPARE(option.getNumber(), Information::Observe); 
    QCOMPARE(coap_decode_var_bytes(option.getValue(), option.getLength()), COAP_OBSERVE_ESTABLISH);

    /* 测试Option的getData()函数 */
    coap_opt_filter_t ignore_options;
    coap_option_filter_clear(&ignore_options);
    coap_option_filter_set(&ignore_options, COAP_OPTION_URI_PATH);
    coap_option_iterator_init(pdu, &opt_iter, &ignore_options);
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getData().data(), option.getData().size()), "coap:");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getData().data(), option.getData().size()), "");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getData().data(), option.getData().size()), "[::1]:40288");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getData().data(), option.getData().size()), "coapcpp");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getData().data(), option.getData().size()), "test");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getData().data(), option.getData().size()), "pdu");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getValue(), option.getLength()), "option");

    coap_delete_pdu(pdu);
}

void tst_Pdu::test_Options()
{
    Options options;
    std::string uri = "coap://[::1]:40288/coapcpp/test/pdu/options?a=1&b=2";

    QVERIFY(options.insertOsberveOption(false));
    QVERIFY(options.insertURIOption(uri));
    QVERIFY(options.insertContentFormatOption(Information::ContentFormatType::TextPlain));

    auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, m_session);
    QVERIFY(pdu);

    ResponsePdu respone(pdu);
    QVERIFY(respone.addOptions(options));
    Pdu::LogPdu(LOG_INFO, &respone);
    coap_opt_iterator_t opt_iter;
    coap_opt_filter_t ignore_options;
    coap_option_filter_clear(&ignore_options);
    coap_option_filter_set(&ignore_options, Information::Observe);
    coap_option_iterator_init(pdu, &opt_iter, &ignore_options);
    Option option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(option.getNumber(), Information::Observe);
    QCOMPARE(coap_decode_var_bytes(option.getValue(), option.getLength()), COAP_OBSERVE_CANCEL);
    coap_option_filter_clear(&ignore_options);
    coap_option_filter_set(&ignore_options, Information::ContentFormat);
    QVERIFY(coap_option_iterator_init(pdu, &opt_iter, &ignore_options));
    Option option1((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(option1.getNumber(), Information::ContentFormat);
    QCOMPARE(coap_decode_var_bytes(option1.getValue(), option1.getLength()), Information::TextPlain);
    coap_option_filter_clear(&ignore_options);
    coap_option_filter_set(&ignore_options, Information::UriPort);
    coap_option_iterator_init(pdu, &opt_iter, &ignore_options);
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(option.getNumber(), Information::UriPort);
    QCOMPARE(coap_decode_var_bytes(option.getValue(), option.getLength()), 40288);
    coap_option_filter_clear(&ignore_options);
    coap_option_filter_set(&ignore_options, Information::UriPath);
    coap_option_iterator_init(pdu, &opt_iter, &ignore_options);
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getValue(), option.getLength()), "coapcpp");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getValue(), option.getLength()), "test");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getValue(), option.getLength()), "pdu");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getValue(), option.getLength()), "options");
    coap_option_filter_clear(&ignore_options);
    coap_option_filter_set(&ignore_options, Information::UriQuery);
    coap_option_iterator_init(pdu, &opt_iter, &ignore_options);
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getValue(), option.getLength()), "a=1");
    option = Option((Information::OptionNumber)opt_iter.number, coap_option_next(&opt_iter));
    QCOMPARE(QString::fromUtf8((const char*)option.getValue(), option.getLength()), "b=2");

    auto query = coap_get_query(pdu);
    QCOMPARE(QString::fromUtf8((const char*)query->s, query->length), "a=1&b=2");
    coap_delete_pdu(pdu);
    coap_delete_string(query);
}

void tst_Pdu::test_Options1()
{
    auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, m_session);
    QVERIFY(pdu);

    // 正常插入
    Options options;
    std::string uri = "coap://[::1]:40288/coapcpp/test/pdu/options?a=1&b=2";
    QVERIFY(options.insertOsberveOption(false));
    QVERIFY(options.insertURIOption(uri));
    QVERIFY(options.insertContentFormatOption(Information::ContentFormatType::Json));
    ResponsePdu respone(pdu);
    QVERIFY(respone.addOptions(options));

    // 重复插入，虽然能插入，但是不会生效
    Options options1;
    QVERIFY(options1.insertContentFormatOption(Information::ContentFormatType::Xml));
    QVERIFY(respone.addOptions(options1));
    
    // 测试后来插入的值不会生效
    coap_opt_iterator_t oi;
    auto opt = coap_check_option(pdu, Information::ContentFormat, &oi);
    QVERIFY(opt);
    Option option((Information::OptionNumber)oi.number, opt);
    QCOMPARE(Decoder::Decode(option.getData()), Information::ContentFormatType::Json);

    coap_show_pdu(LOG_INFO, pdu);
    coap_delete_pdu(pdu);
}

void tst_Pdu::test_Options2()
{
    auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, m_session);
    QVERIFY(pdu);

    // 正常插入
    Options options;
    std::string uri = "coap://[::1]:40288/coapcpp/test/pdu/options?a=1&b=2";
    QVERIFY(options.insertURIOption(uri));
    QVERIFY(options.insertContentFormatOption(Information::ContentFormatType::TextPlain));
    ResponsePdu respone(pdu);
    QVERIFY(respone.addOptions(options));

    // 插入payload
    QCOMPARE(coap_add_data(pdu, 7, (const uint8_t*)"coapcpp"), 1);
    QCOMPARE(coap_add_data(pdu, 4, (const uint8_t*)"test"), 0);
    const uint8_t* data = nullptr;
    size_t length;
    QCOMPARE(coap_get_data(pdu, &length, &data), 1);
    QCOMPARE(QString::fromUtf8((const char*)data, length), "coapcpp");

    // 测试插入payload之后，再插入option，会失败
    Options options1;
    QVERIFY(options1.insertOsberveOption(false));
    QVERIFY(!respone.addOptions(options1));

    coap_show_pdu(LOG_INFO, pdu);
    coap_delete_pdu(pdu);
}   

void tst_Pdu::test_OptFilter()
{
    std::string path = "coap://[::1]:40288/coapcpp/test/pdu/OptFilter?a=1&b=2";
    coap_uri_t uri;
    coap_optlist_t *optlist = nullptr;
    QVERIFY2( !coap_split_uri((uint8_t *)(path.c_str()), path.size(), &uri), "coap_split_uri failed" );
    uint8_t buf[100];
    QVERIFY2( !coap_uri_into_options(&uri, &optlist, 1, buf, sizeof(buf)), "coap_uri_into_options failed" );
    coap_insert_optlist(&optlist, coap_new_optlist(Information::ContentFormat, coap_encode_var_safe(buf, sizeof(buf), Information::Xml), buf));
    auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, m_session);
    QVERIFY2(coap_add_optlist_pdu(pdu, &optlist), "coap_add_optlist_pdu failed");
    coap_delete_optlist(optlist);
    ResponsePdu respone(pdu);

    OptFilter filter({ Information::Observe});
    auto obsVec = respone.getOptions(filter);
    QVERIFY2( obsVec.size() == 0, "没有Observe" );
    
    filter = OptFilter({ Information::ContentFormat, Information::UriPort });
    auto vec = respone.getOptions(filter);
    QCOMPARE(vec.size(), 2);
    QCOMPARE(vec[0].getNumber(), Information::UriPort);
    QCOMPARE(vec[1].getNumber(), Information::ContentFormat);
    coap_delete_pdu(pdu);
}

void tst_Pdu::test_Encoder_Decoder()
{
    coap_optlist_t *optlist_chain = nullptr;
    
    /*** 模拟发送准备 ***/ 
    auto pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, 0, 1024);

    Encoder encoder(COAP_OBSERVE_ESTABLISH);
    QCOMPARE(coap_insert_optlist(&optlist_chain, coap_new_optlist(Information::Observe, encoder.getLength(), encoder.getData().data())), 1);
    encoder = Encoder(Information::ContentFormatType::Xml);
    QCOMPARE(coap_insert_optlist(&optlist_chain, coap_new_optlist(Information::ContentFormat, encoder.getLength(), encoder.getData().data())), 1);
    encoder = Encoder(40288);
    QCOMPARE(coap_insert_optlist(&optlist_chain, coap_new_optlist(Information::UriPort, encoder.getLength(), encoder.getData().data())), 1);
    QVERIFY(coap_add_optlist_pdu(pdu, &optlist_chain));
    coap_delete_optlist(optlist_chain);

    /*** 模拟接收 ***/
    coap_opt_iterator_t opt_iter;
    auto opt = Option((Information::OptionNumber)opt_iter.number, coap_check_option(pdu, Information::Observe, &opt_iter));
    QCOMPARE(Decoder::Decode(opt.getData()), COAP_OBSERVE_ESTABLISH);
    
    opt = Option((Information::OptionNumber)opt_iter.number, coap_check_option(pdu, Information::ContentFormat, &opt_iter));
    QCOMPARE(Decoder::Decode(opt.getData()), Information::ContentFormatType::Xml);

    opt = Option((Information::OptionNumber)opt_iter.number, coap_check_option(pdu, Information::UriPort, &opt_iter));
    QCOMPARE(Decoder::Decode(opt.getData()), 40288);
}

void tst_Pdu::test_token()
{
    int mid = 0x01;
    auto pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, mid, 1024);
    QVERIFY(pdu);
    size_t size;
    uint8_t tokenData[8];
    uint8_t* emptyTokenData = nullptr;
    coap_session_new_token(m_session, &size, tokenData);
    QVERIFY(tokenData);

    auto token = BinaryConst::Create(size, tokenData);
    BinaryConstView tokenView(token);
    QVERIFY_EXCEPTION_THROWN(BinaryConst::Create(0, emptyTokenData), std::invalid_argument);
    
    RequestPdu request(pdu, tokenView);
    QCOMPARE(request.token(), tokenView);
    QVERIFY(request.token() == tokenView);
    coap_delete_pdu(pdu);
}

void tst_Pdu::test_requestPdu()
{
    int mid = 0x02;
    auto pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, mid, 1024);
    QVERIFY(pdu);
    size_t size;
    uint8_t tokenData[8];
    coap_session_new_token(m_session, &size, tokenData);
    QVERIFY(tokenData);
    auto token = BinaryConst::Create(size, tokenData);
    BinaryConstView tokenView(token);
    RequestPdu request(pdu, tokenView);

    // 测试RequestPdu初始化是否正常
    QCOMPARE(request.messageType(), Information::Confirmable);
    QCOMPARE(request.code(), Information::Get);
    QCOMPARE(request.messageId(), mid);
    QCOMPARE(request.payload(), Payload());
    QCOMPARE(request.token(), tokenView);

    // 测试RequestPdu的setCode和setMessageType
    request.setCode(Information::Post);
    QCOMPARE(request.code(), Information::Post);
    request.setMessageType(Information::NonConfirmable);
    QCOMPARE(request.messageType(), Information::NonConfirmable);

    // 测试RequestPdu的addOptions
    Options options;
    QVERIFY(!request.addOptions(options));
    QVERIFY_EXCEPTION_THROWN(options.insertURIOption("coap:///coapcpp/test/pdu/requestPdu"), std::invalid_argument);
    options.insertURIOption("/coapcpp/test/pdu/requestPdu");
    options.insertOsberveOption(true);
    QVERIFY(request.addOptions(options));

    // 测试RequestPdu的isContainOption
    QVERIFY(request.isContainOption(Information::Observe));
    QVERIFY(!request.isContainOption(Information::UriPort));
    QVERIFY(request.isContainOption(Information::UriPath));

    // 测试RequestPdu的getOptions()
    auto vec = request.getOptions();
    QCOMPARE(vec.size(), 5);
    QCOMPARE(Decoder::Decode(vec[0].getData()), COAP_OBSERVE_ESTABLISH);
    QCOMPARE(QString::fromUtf8((const char*)vec[1].getValue(), vec[1].getLength()), "coapcpp");
    QCOMPARE(QString::fromUtf8((const char*)vec[2].getValue(), vec[2].getLength()), "test");
    QCOMPARE(QString::fromUtf8((const char*)vec[3].getValue(), vec[3].getLength()), "pdu");
    QCOMPARE(QString::fromUtf8((const char*)vec[4].getValue(), vec[4].getLength()), "requestPdu");

    // 测试RequestPdu的getOptions(OptionNumber)
    auto vec2 = request.getOptions(OptFilter(std::vector{Information::Observe}));
    QCOMPARE(vec2.size(), 1);
    QCOMPARE(Decoder::Decode(vec2[0].getData()), COAP_OBSERVE_ESTABLISH);
    
    // 测试RequestPdu的setPayload, 没有添加ContentFormat选项
    Payload emptypayload;
    QVERIFY(!request.setPayload(emptypayload));
    auto data = QByteArray::number(1234, 10);
    Payload payload(data.size(), (uint8_t*)data.data(), Information::OctetStream);
    QCOMPARE(payload.size(), data.size());
    QVERIFY(request.setPayload(payload));
    QVERIFY(request.isContainOption(Information::ContentFormat));
    QCOMPARE(Decoder::Decode(request.getOptions(OptFilter(std::vector{Information::ContentFormat}))[0].getData()), Information::OctetStream);
    QVERIFY(!request.setPayload(payload));

    // 测试RequestPdu的payload
    QCOMPARE(request.payload(), payload);
    auto payloadData = QByteArray((const char*)request.payload().data().data(), request.payload().size());
    QCOMPARE(payloadData.toInt(), 1234);

    Pdu::LogPdu(LOG_INFO, &request);
    coap_delete_pdu(pdu);
}

void tst_Pdu::test_responsePdu()
{
    int mid = 0x03;
    auto pdu = coap_pdu_init(COAP_MESSAGE_ACK, COAP_RESPONSE_CODE_CONTENT, mid, 1024);
    QVERIFY(pdu);
    ResponsePdu response(pdu);

    // 测试ResponsePdu初始化是否正常
    QCOMPARE(response.messageType(), Information::Acknowledgement);
    QCOMPARE(response.code(), Information::Content);
    QCOMPARE(response.messageId(), mid);
    QCOMPARE(response.payload(), Payload());

    // 测试ResponsePdu的setCode和setMessageType
    response.setCode(Information::Created);
    QCOMPARE(response.code(), Information::Created);
    response.setMessageType(Information::Reset);
    QCOMPARE(response.messageType(), Information::Reset);

    // 测试ResponsePdu的addOptions
    Options options;
    QVERIFY(!response.addOptions(options));
    QVERIFY_EXCEPTION_THROWN(options.insertURIOption("coapcpp/test/pdu/ResponsePdu"), std::invalid_argument);
    options.insertURIOption("coap://[::]:40288/coapcpp/test/pdu/ResponsePdu");
    options.insertContentFormatOption(Information::Json);
    options.insertOsberveOption(false);
    QVERIFY(response.addOptions(options));

    // 测试ResponsePdu的isContainOption
    QVERIFY(response.isContainOption(Information::Observe));
    QVERIFY(response.isContainOption(Information::UriPort));
    QVERIFY(response.isContainOption(Information::UriPath));
    QVERIFY(response.isContainOption(Information::ContentFormat));

    // 测试ResponsePdu的getOptions()
    auto vec = response.getOptions();
    QCOMPARE(vec.size(), 7);
    QCOMPARE(Decoder::Decode(vec[0].getData()), COAP_OBSERVE_CANCEL);
    QCOMPARE(Decoder::Decode(vec[1].getData()), 40288);
    QCOMPARE(QString::fromUtf8((const char*)vec[2].getValue(), vec[2].getLength()), "coapcpp");
    QCOMPARE(QString::fromUtf8((const char*)vec[3].getValue(), vec[3].getLength()), "test");
    QCOMPARE(QString::fromUtf8((const char*)vec[4].getValue(), vec[4].getLength()), "pdu");
    QCOMPARE(QString::fromUtf8((const char*)vec[5].getValue(), vec[5].getLength()), "ResponsePdu");
    QCOMPARE(Decoder::Decode(vec[6].getData()), Information::Json);

    // 测试ResponsePdu的getOptions(OptionNumber)
    auto vec2 = response.getOptions(OptFilter(std::vector{Information::ContentFormat}));
    QCOMPARE(vec2.size(), 1);
    QCOMPARE(Decoder::Decode(vec2[0].getData()), Information::Json);
    
    // 测试ResponsePdu的setPayload, 没有添加ContentFormat选项
    Payload emptypayload;
    QVERIFY(!response.setPayload(emptypayload));
    auto map = QVariantMap {
        {"test", "response"},
        {"ContentFormat", "Json"}
    };
    QByteArray data = QJsonDocument::fromVariant(map).toJson();
    Payload payload(data.size(), (uint8_t*)data.data(), Information::Json);
    QCOMPARE(payload.size(), data.size());
    QVERIFY(response.setPayload(payload));
    QVERIFY(response.isContainOption(Information::ContentFormat));
    QCOMPARE(Decoder::Decode(response.getOptions(OptFilter(std::vector{Information::ContentFormat}))[0].getData()), Information::Json);
    QVERIFY(!response.setPayload(payload));

    // 测试ResponsePdu的payload
    QCOMPARE(response.payload(), payload);
    auto payloadData = QByteArray((const char*)response.payload().data().data(), response.payload().size());
    QVERIFY(QJsonDocument::fromJson(payloadData).isObject());
    QVERIFY(QJsonDocument::fromJson(payloadData).object().value("test").toString() == "response");
    QVERIFY(QJsonDocument::fromJson(payloadData).object().value("ContentFormat").toString() == "Json");

    Pdu::LogPdu(LOG_INFO, &response);
    coap_delete_pdu(pdu);
}

QTEST_MAIN(tst_Pdu)

#include "tst_Pdu.moc"
