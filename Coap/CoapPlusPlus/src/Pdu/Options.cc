#include "Options.h"
#include <coap3/coap.h>
#include "coap/exception.h"
#include <iostream>
namespace CoapPlusPlus
{

Options::Options(Information::OptionNumber number, std::vector<uint8_t> data)
{
    auto value = data.data();
    auto length = data.size();
    m_optList = coap_new_optlist(number, length, value);
    if(m_optList == nullptr)
        throw std::invalid_argument("Can't construct Options object, invalid option data");
}

Options::Options(const Options &other)
{  
    for(auto node = other.m_optList; node != nullptr; node = node->next) {
        // 必须在这里给optlist->data分配内存，因为coap_delete_optlist 仅仅会释放optlist本身的内存，不会释放optlist->data指向的内存
        auto optlist = (coap_optlist_t*)coap_malloc(sizeof(coap_optlist_t) + node->length);
        optlist->number = node->number;
        optlist->length = node->length;
        optlist->data = (uint8_t*)(optlist + 1); // optlist->data 指向 optlist 后面的内存
        memcpy(optlist->data, node->data, node->length);
        coap_insert_optlist(&m_optList, optlist);
    }
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
    return !coap_uri_into_options(&uri, nullptr, &m_optList, 1, buf, sizeof(buf));
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

void Options::deleteOptList() noexcept
{
    coap_delete_optlist(m_optList); 
    m_optList = nullptr; 
}


}; // namespace CoapPlusPlus