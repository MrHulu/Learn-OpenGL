#include <coap3/coap.h>
#include "BinaryView.h"
#include <cstring>

namespace CoapPlusPlus {

bool BinaryView::operator<(const BinaryView &other) const
{
    if (m_rawData->length < other.m_rawData->length) 
        return true; 
    else if (m_rawData->length > other.m_rawData->length)
        return false;
    else
        return std::memcmp(m_rawData->s, other.m_rawData->s, m_rawData->length) < 0;
}

size_t BinaryView::size() const
{ 
    if(m_rawData == nullptr)
        throw DataWasReleasedException("");
    return m_rawData->length; 
}

std::span<uint8_t> BinaryView::data() const
{ 
    if(m_rawData == nullptr)
        throw DataWasReleasedException("");
    return std::span<uint8_t>(const_cast<uint8_t*>(m_rawData->s), m_rawData->length);
}

} // namespace CoapPlusPlus