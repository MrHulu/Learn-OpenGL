#include <QtTest>
#include <QSignalSpy>
#include <QDebug>
#include <QString>

#include "coap/Pdu/Encoder.h"
#include "coap/Pdu/Decoder.h"
#include "coap/Pdu/Option.h"
#include "coap/Pdu/Options.h"
#include "coap/Pdu/OptFilter.h"

using namespace CoapPlusPlus;

class tst_Pdu : public QObject
{
    Q_OBJECT
public:
    tst_Pdu() {
        m_context = coap_new_context(nullptr);
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

    void test_OptFilter(); // 测试OptFilter类

    void test_Encoder_Decoder(); // 测试Encoder和Decoder类

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
    Options options(nullptr);
    std::string uri = "coap://[::1]:40288/coapcpp/test/pdu/options?a=1&b=2";

    QVERIFY(options.insertOsberveOption(false));
    QVERIFY(options.insertURIOption(uri));
    QVERIFY(options.insertContentFormatOption(Information::ContentFormatType::TextPlain));

    auto pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, m_session);
    QVERIFY(pdu);

    // TODO: getOptList()应该是私有的，需要修改，这里为了测试, 等实现了Pdu类再修改
    auto optlist = options.getOptList();
    QVERIFY(coap_add_optlist_pdu(pdu, &optlist));
    coap_delete_optlist(optlist);
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
    coap_option_iterator_init(pdu, &opt_iter, &ignore_options);
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

    // TODO: getOptFilter()应该是私有的，需要修改，这里为了测试， 等实现了Pdu类再修改
    coap_opt_iterator_t opt_iter;
    OptFilter filter({ Information::Observe});
    coap_option_iterator_init(pdu, &opt_iter, filter.getOptFilter());
    QVERIFY2( coap_option_next(&opt_iter) == nullptr, "没有Observe" );
    filter = OptFilter({ Information::ContentFormat, Information::UriPort });
    coap_option_iterator_init(pdu, &opt_iter, filter.getOptFilter());
    QVERIFY(coap_option_next(&opt_iter)); QCOMPARE(opt_iter.number, Information::UriPort);
    QVERIFY(coap_option_next(&opt_iter)); QCOMPARE(opt_iter.number, Information::ContentFormat);
    QVERIFY( coap_option_next(&opt_iter) == nullptr );

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

QTEST_MAIN(tst_Pdu)

#include "tst_Pdu.moc"
