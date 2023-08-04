#include <QtTest>
#include <QSignalSpy>
#include <QDebug>

#include <coap3/coap.h>
#include "coap/DataStruct/Binary.h"
#include "coap/DataStruct/BinaryView.h"
#include "coap/DataStruct/BinaryConst.h"
#include "coap/DataStruct/BinaryConstView.h"
#include "coap/DataStruct/Address.h"

using namespace CoapPlusPlus;

class tst_DataStruct : public QObject {
    Q_OBJECT
public:
    tst_DataStruct() { 
        coap_startup();
        coap_set_log_level(LOG_DEBUG); 
    }
    ~tst_DataStruct() { 
        coap_cleanup();
    }
private:
    
private slots:
    void binary_test();
    
    void binaryView_test();

    void binaryConst_test();

    void binaryConstView_test();

    void test_address_Construct();
    
    void test_address_Operator();

    void test_address_Invalid();
};

void tst_DataStruct::binary_test()
{
    const char* data = "Hello, binary_test!";
    coap_binary_t* raw_binary = coap_new_binary(strlen(data));
    memcpy(raw_binary->s, data, raw_binary->length);

    auto binary = Binary::DeepCopy(raw_binary);
    QCOMPARE(binary.size(), raw_binary->length);
    QCOMPARE(memcmp(binary.data().data(), raw_binary->s, binary.size()), 0);

    Binary binary2 = Binary::Create(raw_binary->length, raw_binary->s);
    QCOMPARE(binary2.size(), raw_binary->length);
    QCOMPARE(memcmp(binary2.data().data(), raw_binary->s, binary2.size()), 0);

    Binary binary3 = Binary::Reference(raw_binary);
    QCOMPARE(binary3.size(), raw_binary->length);
    QCOMPARE(memcmp(binary3.data().data(), raw_binary->s, binary3.size()), 0);

    coap_delete_binary(raw_binary);
}

void tst_DataStruct::binaryView_test()
{
    const char* data = "Hello, binaryView_test!";
    auto raw_binary = coap_new_binary(strlen(data));
    memcpy(raw_binary->s, data, raw_binary->length);

    BinaryView binaryView(raw_binary);
    QCOMPARE(binaryView.size(), raw_binary->length);
    QCOMPARE(memcmp(binaryView.data().data(), raw_binary->s, binaryView.size()), 0);

    Binary binary = binaryView.toBinary();
    QCOMPARE(binary.size(), raw_binary->length);
    QCOMPARE(memcmp(binary.data().data(), raw_binary->s, binary.size()), 0);

    coap_delete_binary(raw_binary);
}

void tst_DataStruct::binaryConst_test()
{
    const char* data = "Hello, binaryConst_test!";
    auto raw_binary = coap_new_bin_const((uint8_t*)data, strlen(data));

    auto binaryConst = BinaryConst::DeepCopy(raw_binary);
    QCOMPARE(binaryConst.size(), raw_binary->length);
    QCOMPARE(memcmp(binaryConst.data().data(), raw_binary->s, binaryConst.size()), 0);

    auto binary2 = BinaryConst::Create(raw_binary->length, raw_binary->s);
    QCOMPARE(binary2.size(), raw_binary->length);
    QCOMPARE(memcmp(binary2.data().data(), raw_binary->s, binary2.size()), 0);

    auto binary3 = BinaryConst::Reference(raw_binary);
    QCOMPARE(binary3.size(), raw_binary->length);
    QCOMPARE(memcmp(binary3.data().data(), raw_binary->s, binary3.size()), 0);

    coap_delete_bin_const(raw_binary);
}

void tst_DataStruct::binaryConstView_test()
{
    const char* data = "Hello, binaryConstView_test!";
    auto raw_binary = coap_new_bin_const((uint8_t*)data, strlen(data));

    BinaryConstView binaryConstView(raw_binary);
    QCOMPARE(binaryConstView.size(), raw_binary->length);
    QCOMPARE(memcmp(binaryConstView.data().data(), raw_binary->s, binaryConstView.size()), 0);

    auto binaryConst = binaryConstView.toBinaryConst();
    QCOMPARE(binaryConst.size(), raw_binary->length);
    QCOMPARE(memcmp(binaryConst.data().data(), raw_binary->s, binaryConst.size()), 0);

    coap_delete_bin_const(raw_binary);
}

void tst_DataStruct::test_address_Construct()
{
    // sockaddr_in 
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(40288);
    inet_pton(AF_INET, "192.0.2.0", &(address.sin_addr));
    Address addr(address);
    QCOMPARE(addr.getIpAddress(), std::string("192.0.2.0"));
    QCOMPARE(addr.getPort(), 40288);

    // sockaddr_in6
    sockaddr_in6 address6;
    address6.sin6_family = AF_INET6;
    address6.sin6_port = htons(40288);
    inet_pton(AF_INET6, "2001:db8::", &(address6.sin6_addr));
    Address addr6(address6);
    QCOMPARE(addr6.getIpAddress(), std::string("2001:db8::"));
    QCOMPARE(addr6.getPort(), 40288);

    // ip port CASE 1
    Address addr_ip_port1(std::string("localhost"), 40288); 
    /* localhost被映射为IPv6地址::1。IPv6使用::1作为循环回环地址，与IPv4中的127.0.0.1等效。
     * 因此，当你将localhost传递给Address构造函数时，它会解析为IPv6地址::1。 
     */
    QCOMPARE(addr_ip_port1.getIpAddress(), std::string("::1"));
    QCOMPARE(addr_ip_port1.getPort(), 40288);

    // ip port CASE 2
    Address addr_ip_port2(std::string("::"), 40288);
    QCOMPARE(addr_ip_port2.getIpAddress(), std::string("::"));
    QCOMPARE(addr_ip_port2.getPort(), 40288);

    // ip port CASE 3
    Address addr_ip_port3(std::string("127.0.0.1"), 40288);
    QCOMPARE(addr_ip_port3.getIpAddress(), std::string("127.0.0.1"));
    QCOMPARE(addr_ip_port3.getPort(), 40288);

    // ip port CASE 4
    Address addr_ip_port4(std::string("0.0.0.0"), 40288);
    QCOMPARE(addr_ip_port4.getIpAddress(), std::string("0.0.0.0"));
    QCOMPARE(addr_ip_port4.getPort(), 40288);

    // coap_address_t
    coap_address_t raw_address;
    coap_address_init(&raw_address);
    raw_address.size = sizeof(raw_address.addr.sin);
    raw_address.addr.sin.sin_family = AF_INET;
    raw_address.addr.sin.sin_port = htons(40288);
    raw_address.addr.sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    Address addr_coap(raw_address);
    QCOMPARE(addr_coap.getIpAddress(), std::string("127.0.0.1"));
    QCOMPARE(addr_coap.getPort(), 40288);

    // copy
    Address originalCopy(std::string("192.168.5.149"), 40288);
    Address copy(originalCopy);
    QCOMPARE(copy.getIpAddress(), originalCopy.getIpAddress());
    QCOMPARE(copy.getPort(), originalCopy.getPort());

    // move
    Address originalMove(std::string("192.0.2.0"), 12345);
    Address moved(std::move(originalMove));
    QCOMPARE(moved.getIpAddress(), std::string("192.0.2.0"));
    QCOMPARE(moved.getPort(), 12345);
}

void tst_DataStruct::test_address_Operator()
{
    // copy 
    Address original(std::string("192.0.3.3"), 60188);
    Address copy = original;
    QCOMPARE(copy.getIpAddress(), original.getIpAddress());
    QCOMPARE(copy.getPort(), original.getPort());

    // move
    Address moved = std::move(original);
    QCOMPARE(moved.getIpAddress(), std::string("192.0.3.3"));
    QCOMPARE(moved.getPort(), 60188);
}

void tst_DataStruct::test_address_Invalid()
{
    QVERIFY_EXCEPTION_THROWN(Address(std::string("not.an.ip.address"), 12345), std::invalid_argument);
    
    coap_address_t address;
    coap_address_init(&address);
    address.size = 0;
    QVERIFY_EXCEPTION_THROWN(Address addr(address), std::invalid_argument);
}

QTEST_MAIN(tst_DataStruct)

#include "tst_DataStruct.moc"

