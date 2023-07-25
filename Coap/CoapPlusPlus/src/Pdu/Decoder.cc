#include <coap3/coap.h>
#include "Decoder.h"

namespace CoapPlusPlus
{

uint32_t Decoder::Decode(std::vector<uint8_t> data) noexcept
{ 
    auto value = data.data();
    auto length = data.size();
    return coap_decode_var_bytes(value, length);
}

uint64_t Decoder::Decode8(std::vector<uint8_t> data) noexcept
{
    auto value = data.data();
    auto length = data.size();
    return coap_decode_var_bytes8(value, length);
}

}; // namespace CoapPlusPlus