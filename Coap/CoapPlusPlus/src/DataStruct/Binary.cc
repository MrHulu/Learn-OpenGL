#include "Binary.h"
#include "coap/exception.h"

#include <stdexcept>

namespace CoapPlusPlus {


Binary::Binary(coap_binary_t* raw, bool owned)
{
    if(raw == nullptr)
        throw std::invalid_argument("raw cannot be null");
    m_rawData = raw;
    m_owned = owned;
}

Binary::~Binary()
{
    if(m_rawData != nullptr && m_owned)
        coap_delete_binary(m_rawData);
}

Binary Binary::Reference(const coap_binary_t* raw)
{
    return Binary(const_cast<coap_binary_t*>(raw), false);
}

Binary Binary::DeepCopy(coap_binary_t* raw)
{
    if(raw == nullptr)
        throw std::invalid_argument("raw cannot be null");
    return Create(raw->length, raw->s);
}

Binary Binary::Create(size_t size, uint8_t *data)
{
    if(data == nullptr)
        throw std::invalid_argument("data cannot be null");
    auto raw = coap_new_binary(size);
    if(raw == nullptr)
        throw CallCoapLibFuncException("coap_new_binary failed");
    std::copy(data, data + size, raw->s);
    return Binary(raw, true);
}

size_t Binary::size() const
{
    if(m_rawData == nullptr)
        throw DataWasReleasedException("data was released");
    return m_rawData->length;
}

std::span<uint8_t> Binary::data() const
{
    if(m_rawData == nullptr)
        throw DataWasReleasedException("data was released");
    return std::span<uint8_t>(m_rawData->s, m_rawData->length);
}

};// namespace CoapPlusPlus PlusPlus