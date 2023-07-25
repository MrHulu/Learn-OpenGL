#include <coap3/coap.h>
#include "Encoder.h"

namespace CoapPlusPlus
{

Encoder::Encoder(uint32_t value)
{
    uint8_t buf[4] = {0};
    m_data.clear();
    m_length = coap_encode_var_safe(buf, sizeof(buf), value);
    if(m_length != 0 && m_length <= sizeof(buf)) {
        m_data.reserve(m_length);
        m_data.insert(m_data.end(), buf, buf + m_length);
    }else if(m_length == 0 && m_length <= sizeof(buf)) {

    }else {
        throw std::invalid_argument("Invalid value for encoding.");
    }
}

Encoder::Encoder(uint64_t value)
{
    uint8_t buf[8] = {0};
    m_data.clear();
    m_length = coap_encode_var_safe8(buf, sizeof(buf), value);
    if(m_length != 0 && m_length <= sizeof(buf)) {
        m_data.reserve(m_length);
        m_data.insert(m_data.end(), buf, buf + m_length);
    }else if(m_length == 0 && m_length <= sizeof(buf)) {

    }else {
        throw std::invalid_argument("Invalid value for encoding.");
    }
}

}; // namespace CoapPlusPlus