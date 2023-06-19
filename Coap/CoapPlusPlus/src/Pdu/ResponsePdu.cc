#include "ResponsePdu.h"
#include "Options.h"
#include "Encoder.h"

namespace CoapPlusPlus {

ResponsePdu::ResponsePdu(coap_pdu_t *pdu)
    : Pdu(pdu)
{
    if(pdu == nullptr)
        throw std::invalid_argument("Can't construct ResponsePdu object, pdu is nullptr");
    
    m_responseCode = static_cast<ResponseCode>(coap_pdu_get_code(pdu));
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
    if(data.size() > 0 && payload.type() != Information::None){
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
        coap_log_err("[ResponsePdu::setPayload]payload is empty or type is None");
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

}; // namespace CoapPlusPlus