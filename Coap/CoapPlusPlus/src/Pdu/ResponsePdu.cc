#include <coap3/coap.h>
#include "ResponsePdu.h"
#include "coap/DataStruct/BinaryConstView.h"
#include "Options.h"
#include "Option.h"
#include "OptFilter.h"
#include "Encoder.h"
#include "Decoder.h"
#include "Payload.h"

namespace CoapPlusPlus {

ResponsePdu::ResponsePdu(coap_pdu_t *pdu)
    : Pdu(pdu)
{
    init();
}

ResponsePdu::~ResponsePdu()
{
}

Payload ResponsePdu::payload() const noexcept
{
    if(m_payload.has_value())
        return m_payload.value();
    return Payload();
}

bool ResponsePdu::setPayload(Payload payload) noexcept
try{
    if(m_rawPdu == nullptr || m_payload.has_value())
        return false;
    auto data = payload.data();
    if(data.size() > 0 && payload.type() != Information::NoneType){
        // 检查选项列表中是否包含该数据的格式的option， 没有就补上去
        if(isContainOption(Information::ContentFormat) == false){
            Encoder encoder(payload.type());
            Options options(Options(Information::ContentFormat, encoder.getData()));
            if(addOptions(options))
                return false;
        }
        m_payload = payload;
        coap_add_data(m_rawPdu, data.size(), data.data());
        return true;
    }
    else {
        coap_log_err("[ResponsePdu::setPayload]payload is empty or type is None\n");
        return false;
    }
}catch(std::exception &e){
    coap_log_err(e.what());
    return false;
}

bool ResponsePdu::setCode(ResponseCode code) noexcept
{
    if(m_rawPdu == nullptr)
        return false;
    if(code != m_responseCode){
        coap_pdu_set_code(m_rawPdu, static_cast<coap_pdu_code_t >(code));
        m_responseCode = code;
    }
    return true;
}

BinaryConstView ResponsePdu::requestToken() const noexcept
{
    if(m_rawPdu == nullptr)
        return BinaryConstView(nullptr);
    auto coap_token = coap_pdu_get_token(m_rawPdu);
    return BinaryConstView(&coap_token);
}

void ResponsePdu::init() noexcept
try{
    if(m_rawPdu == nullptr)
        return;
    // code
    m_responseCode = static_cast<ResponseCode>(coap_pdu_get_code(m_rawPdu));

    // type 已在父类实现
    // token requestToken()
    // payload
    if(m_payload.has_value() == false){
        size_t pdu_data_length;
        const uint8_t *pdu_data;
        auto data = coap_get_data(m_rawPdu, &pdu_data_length, &pdu_data);
        if(data != 0){
            auto opts = getOptions(OptFilter(std::vector<Information::OptionNumber>{Information::ContentFormat}));
            if(opts.size() > 0) {
                auto value = static_cast<Information::ContentFormatType>(Decoder::Decode(opts[0].getData()));
                m_payload = Payload(pdu_data_length, pdu_data, value);
            }
        }
    }
}catch(std::exception &e){
    coap_log_err("ResponsePdu::init():%s", e.what());
}

}; // namespace CoapPlusPlus