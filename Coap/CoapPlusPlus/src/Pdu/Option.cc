#include "Option.h"
#include <coap3/coap.h>
#include <stdexcept>

namespace CoapPlusPlus
{

Option::Option(Information::OptionNumber number, coap_opt_t *opt)
    : m_number(number), m_opt(opt)
{
    if(m_opt == nullptr)
        throw std::invalid_argument("opt is invalid");
}

size_t Option::getSize() const noexcept
{
    return coap_opt_size(m_opt);
}

size_t Option::getLength() const noexcept
{
    return coap_opt_length(m_opt);
}

const uint8_t *Option::getValue() const noexcept
{
    return coap_opt_value(m_opt);
}

}; // namespace CoapPlusPlus