#include "BinaryConst.h"
#include "coap/exception.h"

#include <coap3/coap.h>
#include <stdexcept>

namespace CoapPlusPlus {


BinaryConst::BinaryConst(coap_bin_const_t* raw, bool owned)
{
    if(raw == nullptr)
        throw std::invalid_argument("Can't construct BinaryConst, raw cannot be null");
    m_rawData = raw;
    m_owned = owned;
}

bool BinaryConst::operator<(const BinaryConst &other) const
{
    if(m_rawData != nullptr && other.m_rawData != nullptr) {
        if (m_rawData->length < other.m_rawData->length) 
            return true; 
        else if (m_rawData->length > other.m_rawData->length)
            return false;
        else
            return std::memcmp(m_rawData->s, other.m_rawData->s, m_rawData->length) < 0;
    }else{
        return false;
    }
}

bool BinaryConst::operator==(const BinaryConst &other) const noexcept
{
    return coap_binary_equal(m_rawData, other.m_rawData);
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
        throw std::invalid_argument("BinaryConst::DeepCopy(coap_bin_const_t* raw): raw cannot be null");
    return Create(raw->length, raw->s);
}

BinaryConst BinaryConst::Create(size_t size, const uint8_t *data)
{
    if(data == nullptr)
        throw std::invalid_argument("BinaryConst::Create: data cannot be null");
    auto raw = coap_new_bin_const(data, size);
    if(raw == nullptr)
        throw InternalException("BinaryConst::Create: coap_new_bin_const failed");
    return BinaryConst(raw, true);
}

size_t BinaryConst::size() const
{
    if(m_rawData == nullptr)
        throw DataWasReleasedException("BinaryConst::size()：data was released");
    return m_rawData->length;
}

std::span<uint8_t> BinaryConst::data() const
{
    if(m_rawData == nullptr)
        throw DataWasReleasedException("BinaryConst::data()：data was released");
    return std::span<uint8_t>(const_cast<uint8_t*>(m_rawData->s), m_rawData->length);
}

uint64_t BinaryConst::toUInt64() const
 {
    if(m_rawData == nullptr)
        throw DataWasReleasedException("");
    uint64_t result = 0;
    auto len = size();
    for (size_t i = 0; i < len; ++i)
        result |= static_cast<uint64_t>(m_rawData->s[i]) << (8 * (len - i - 1));
    return result;
}

std::string BinaryConst::toHexString() const 
{
    if(m_rawData == nullptr)
        throw DataWasReleasedException("");
    std::string result;
    for (size_t i = 0; i < size(); ++i) {
        char buf[4];
        sprintf_s(buf, "%02X", m_rawData->s[i]); 
        result += buf;
    }
    return result;
}

};// namespace CoapPlusPlus PlusPlus