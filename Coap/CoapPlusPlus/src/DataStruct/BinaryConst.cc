#include "BinaryConst.h"
#include "coap/exception.h"

#include <stdexcept>

namespace CoapPlusPlus {


BinaryConst::BinaryConst(coap_bin_const_t* raw, bool owned)
{
    if(raw == nullptr)
        throw std::invalid_argument("raw cannot be null");
    m_rawData = raw;
    m_owned = owned;
}

BinaryConst::~BinaryConst()
{
    if(m_rawData != nullptr && m_owned)
        coap_delete_bin_const(m_rawData);
}

BinaryConst BinaryConst::Reference(const coap_bin_const_t* raw)
{
    return BinaryConst(const_cast<coap_bin_const_t*>(raw), false);
}

BinaryConst BinaryConst::DeepCopy(coap_bin_const_t* raw)
{
    if(raw == nullptr)
        throw std::invalid_argument("raw cannot be null");
    return Create(raw->length, raw->s);
}

BinaryConst BinaryConst::Create(size_t size, const uint8_t *data)
{
    if(data == nullptr)
        throw std::invalid_argument("data cannot be null");
    auto raw = coap_new_bin_const(data, size);
    if(raw == nullptr)
        throw CallCoapLibFuncException("coap_new_bin_const failed");
    return BinaryConst(raw, true);
}

size_t BinaryConst::size() const
{
    if(m_rawData == nullptr)
        throw DataWasReleasedException("data was released");
    return m_rawData->length;
}

std::span<uint8_t> BinaryConst::data() const
{
    if(m_rawData == nullptr)
        throw DataWasReleasedException("data was released");
    return std::span<uint8_t>(const_cast<uint8_t*>(m_rawData->s), m_rawData->length);
}

};// namespace CoapPlusPlus PlusPlus