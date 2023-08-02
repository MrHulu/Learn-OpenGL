#include "Binary.h"
#include "coap/exception.h"

#include <coap3/coap.h>
#include <stdexcept>
#include <cstring>

namespace CoapPlusPlus {


Binary::Binary(coap_binary_t* raw, bool owned)
{
    if(raw == nullptr)
        throw std::invalid_argument("raw cannot be null");
    m_rawData = raw;
    m_owned = owned;
}

bool Binary::operator<(const Binary &other) const
{
    if (m_rawData->length < other.m_rawData->length) 
        return true; 
    else if (m_rawData->length > other.m_rawData->length)
        return false;
    else
        return std::memcmp(m_rawData->s, other.m_rawData->s, m_rawData->length) < 0;
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
        throw InternalException("coap_new_binary failed");
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