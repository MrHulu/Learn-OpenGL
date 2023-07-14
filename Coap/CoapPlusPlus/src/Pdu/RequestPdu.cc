#include "RequestPdu.h"
#include "Options.h"
#include "Encoder.h"

namespace CoapPlusPlus {

RequestPdu::RequestPdu(coap_pdu_t *rawPdu, BinaryConst token)
    : Pdu(rawPdu)
    , m_token(std::move(token))
{
    m_requestCode = static_cast<RequestCode>(coap_pdu_get_code(rawPdu));
    coap_add_token(rawPdu, m_token.size(), m_token.data().data());
}

RequestPdu::~RequestPdu()
{

}

Payload RequestPdu::payload() const noexcept
{
    if(m_payload.has_value())
        return m_payload.value();
    return Payload();
}

bool RequestPdu::setPayload(Payload payload) noexcept
try{
    if(m_rawPdu == nullptr || m_payload.has_value())
        return false;
    auto data = payload.data();
    if(data.size() > 0 && payload.type() != Information::NoneType){
        // 检查选项列表中是否包含该数据的格式的option， 没有就补上去
        if(isContainOption(Information::ContentFormat) == false){
            Encoder encoder(payload.type());
            Options options(Options(Information::ContentFormat, encoder.getData()));
            if(addOptions(options) == false)
                return false;
        }
        m_payload = payload;
        coap_add_data(m_rawPdu, data.size(), data.data());
        return true;
    }
    else {
        coap_log_err("[RequestPdu::setPayload]payload is empty or type is None\n");
        return false;
    }
}catch(std::exception &e){
    coap_log_err(e.what());
    return false;
}

bool RequestPdu::setCode(RequestCode code) noexcept
{
    if(m_rawPdu == nullptr)
        return false;
    if(code != m_requestCode){
        coap_pdu_set_code(m_rawPdu, static_cast<coap_pdu_code_t >(code));
        m_requestCode = code;
    }
    return true;
}

}; // namespace CoapPlusPlus