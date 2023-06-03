#include <QtTest>
#include <QSignalSpy>
#include <QDebug>

#include "Coap/DataStruct/Binary.h"
#include "Coap/DataStruct/BinaryView.h"
#include "Coap/DataStruct/BinaryConst.h"
#include "Coap/DataStruct/BinaryConstView.h"

using namespace CoapPlusPlus;

class tst_DataStruct : public QObject {
    Q_OBJECT
public:
    tst_DataStruct() { }
    ~tst_DataStruct() {  }
private:
    
private slots:
    void binary_test();
    
    void binaryView_test();

    void binaryConst_test();

    void binaryConstView_test();
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

QTEST_MAIN(tst_DataStruct)

#include "tst_DataStruct.moc"

