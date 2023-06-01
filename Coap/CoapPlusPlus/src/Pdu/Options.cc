#include "Options.h"
#include "coap/exception.h"

namespace CoapPlusPlus
{

Options::Options(coap_optlist_t *optList)
    : m_optList(optList)
{
    if(m_optList == nullptr)
        throw std::invalid_argument("optList is nullptr");
}

bool Options::insert(Information::OptionNumber number, std::vector<uint8_t> data) noexcept
{
    auto value = data.data();
    auto length = data.size();
    return coap_insert_optlist(&m_optList, coap_new_optlist(number, length, value));
}

}; // namespace CoapPlusPlus