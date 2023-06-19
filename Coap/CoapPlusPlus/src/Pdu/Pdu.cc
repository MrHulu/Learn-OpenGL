#include "Pdu.h"
#include "coap/exception.h"
#include "coap/Pdu/Option.h"
#include "coap/Pdu/Options.h"
#include "coap/Pdu/OptFilter.h"

namespace CoapPlusPlus
{

void Pdu::LogPdu(coap_log_t level, const Pdu* pdu) noexcept
{
    if(pdu == nullptr)
        return;
    if(pdu->m_rawPdu == nullptr)
        return;
    coap_show_pdu(level, pdu->m_rawPdu);
}

bool Pdu::setMessageType(MessageType type) noexcept
{
    if(m_rawPdu == nullptr)
        return false;
    if(type != m_type){
        coap_pdu_set_type(m_rawPdu, static_cast<coap_pdu_type_t>(type));
        m_type = type;
    }
    return true;
}

Mid Pdu::messageId() const
{
    if(m_rawPdu == nullptr)
        throw DataWasReleasedException("data was released");
    return coap_pdu_get_mid(m_rawPdu);
}

bool Pdu::isContainOption(OptionNumber number) const
{
    if(m_rawPdu == nullptr)
        throw DataWasReleasedException("data was released");
    coap_opt_iterator_t oi;
    return coap_check_option(m_rawPdu, number, &oi) != nullptr;
}

std::vector<Option> Pdu::getOptions(OptFilter filter) const
{
    if(m_rawPdu == nullptr)
        throw DataWasReleasedException("data was released");
    std::vector<Option> result;
    coap_opt_iterator_t oi;
    coap_option_iterator_init(m_rawPdu, &oi, filter.getOptFilter());
    while(auto rawOpt = coap_option_next(&oi)) {
        try{
            result.emplace_back(Option((OptionNumber)oi.number, rawOpt));
        }catch(const std::exception& e) {}
    }
    return result;
}

bool Pdu::addOptions(Options options)
{
    if(m_rawPdu == nullptr)
        throw DataWasReleasedException("data was released");
    auto optList = options.getOptList();
    auto result = coap_add_optlist_pdu(m_rawPdu, &optList);
    //options.deleteOptList();
    return result;
}

Pdu::Pdu(coap_pdu_t *pdu) : m_rawPdu(pdu)
{
    if(m_rawPdu == nullptr)
        throw std::invalid_argument("pdu is nullptr");
    m_type = static_cast<MessageType>(coap_pdu_get_type(m_rawPdu));
}

}; // namespace CoapPlusPlus