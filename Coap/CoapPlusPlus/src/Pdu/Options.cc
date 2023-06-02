#include "Options.h"
#include "coap/exception.h"
#include <iostream>
namespace CoapPlusPlus
{

Options::Options(coap_optlist_t *optList)
    : m_optList(optList)
{
    
}

bool Options::insert(Information::OptionNumber number, std::vector<uint8_t> data) noexcept
{
    auto value = data.data();
    auto length = data.size();
    return coap_insert_optlist(&m_optList, coap_new_optlist(number, length, value));
}

bool Options::insertURIOption(std::string path)
{
    coap_uri_t uri;
    auto parsePathResult = coap_split_uri((uint8_t *)(path.c_str()), path.size(), &uri);
    if(parsePathResult < 0)
        throw std::invalid_argument("invalid path");
    uint8_t buf[1024]; // TODO: 1024 是否合适？
    return !coap_uri_into_options(&uri, &m_optList, 1, buf, sizeof(buf));
}

bool Options::insertOsberveOption(bool enable) noexcept
{
    uint8_t buf[8];
    auto length = coap_encode_var_safe(buf, sizeof(buf), enable ? COAP_OBSERVE_ESTABLISH : COAP_OBSERVE_CANCEL);
    return coap_insert_optlist(&m_optList, coap_new_optlist(COAP_OPTION_OBSERVE, length, buf));
}

bool Options::insertContentFormatOption(Information::ContentFormatType format) noexcept
{
    uint8_t buf[8];
    auto length = coap_encode_var_safe(buf, sizeof(buf), format);
    return coap_insert_optlist(&m_optList, coap_new_optlist(Information::ContentFormat, length, buf));
}

}; // namespace CoapPlusPlus