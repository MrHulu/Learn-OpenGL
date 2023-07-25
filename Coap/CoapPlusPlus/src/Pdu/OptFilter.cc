#include <coap3/coap.h>
#include "OptFilter.h"


namespace CoapPlusPlus
{

OptFilter::OptFilter(std::vector<Information::OptionNumber> optNumbers)
{
    coap_option_filter_clear(&m_optFilter);
    for (auto optNumber : optNumbers)  {
        coap_option_filter_set(&m_optFilter, optNumber);
    }
}

bool OptFilter::isContain(Information::OptionNumber number) noexcept
{
    return coap_option_filter_get(&m_optFilter, number);
}

bool OptFilter::insert(Information::OptionNumber number) noexcept
{
    return coap_option_filter_set(&m_optFilter, number);;
}

bool OptFilter::remove(Information::OptionNumber number) noexcept
{
    return coap_option_filter_unset(&m_optFilter, number);
}

}; // namespace CoapPlusPlus