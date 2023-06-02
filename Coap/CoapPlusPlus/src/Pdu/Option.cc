#include "Option.h"

#include <stdexcept>

namespace CoapPlusPlus
{

Option::Option(Information::OptionNumber number, coap_opt_t *opt)
    : m_number(number), m_opt(opt)
{
    if(m_opt == nullptr)
        throw std::invalid_argument("opt is invalid");
}

}; // namespace CoapPlusPlus