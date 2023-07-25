#include <coap3/coap.h>
#include "BinaryConstView.h"

namespace CoapPlusPlus
{

bool BinaryConstView::operator==(const BinaryConstView &other) const noexcept
{
    return coap_binary_equal(m_rawData, other.m_rawData);
}

bool BinaryConstView::operator<(const BinaryConstView &other) const
{
    if (m_rawData != nullptr && other.m_rawData != nullptr) {
        if (m_rawData->length < other.m_rawData->length) 
            return true; 
        else if (m_rawData->length > other.m_rawData->length)
            return false;
        else
            return std::memcmp(m_rawData->s, other.m_rawData->s, m_rawData->length) < 0;
    }
    else
        return false;
}

size_t BinaryConstView::size() const
{ 
    if(m_rawData == nullptr)
        throw DataWasReleasedException("");
    return m_rawData->length; 
}

std::span<uint8_t> BinaryConstView::data() const
{ 
    if(m_rawData == nullptr)
        throw DataWasReleasedException("");
    return std::span<uint8_t>(const_cast<uint8_t*>(m_rawData->s), m_rawData->length);
}

uint64_t BinaryConstView::toUInt64() const
{
    if(m_rawData == nullptr)
        throw DataWasReleasedException("");
    uint64_t result = 0;
    auto len = size();
    for (size_t i = 0; i < len; ++i)
        result |= static_cast<uint64_t>(m_rawData->s[i]) << (8 * (len - i - 1));
    return result;
}

std::string BinaryConstView::toHexString() const
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

} // namespace CoapPlusPlus