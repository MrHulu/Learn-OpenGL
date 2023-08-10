
#include "Address.h"
#include "AddressWrapper.h"
namespace CoapPlusPlus
{

Address::Address(const sockaddr_in &address) noexcept
{
    m_Impl = new AddressImpl(address);
}

Address::Address(const sockaddr_in6 &address) noexcept
{
    m_Impl = new AddressImpl(address);
}

Address::Address(const std::string &ip, uint16_t port)
{
    m_Impl = new AddressImpl(ip, port);
}

Address::Address(const coap_address_t &address)
{
    m_Impl = new AddressImpl(address);
}

Address::~Address()
{
    if(m_Impl) {
        delete m_Impl;
        m_Impl = nullptr;
    }
}

Address::Address(const Address & other)
{
    m_Impl = new AddressImpl(other.m_Impl->m_rawAddr); // todo: 是否需要判断m_Impl是否为空
}

Address::Address(Address &&other) : m_Impl(other.m_Impl)
{ 
    other.m_Impl = nullptr;
}

Address &Address::operator=(const Address &other)
{
    if (this != &other) {
        if(m_Impl) {
            delete m_Impl;
            m_Impl = nullptr;
        }
        m_Impl = new AddressImpl(other.m_Impl->m_rawAddr);
    }
    return *this;
}

Address &Address::operator=(Address &&other)
{
    if (this != &other) {
        std::swap(m_Impl, other.m_Impl);
    }
    return *this;
}

bool Address::operator==(const Address &other) const noexcept
{
    if(m_Impl == nullptr || other.m_Impl == nullptr) {
        return true;
    }
    else if(m_Impl && other.m_Impl) {
        return coap_address_equals(&m_Impl->m_rawAddr, &other.m_Impl->m_rawAddr);
    }
    else {
        return false;
    }
}

std::string Address::getIpAddress() const noexcept
{
    auto value = m_Impl->m_rawAddr;
    if (value.size == sizeof(value.addr.sin))
    {
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(value.addr.sin.sin_addr), ipstr, sizeof(ipstr));
        //coap_log_debug("Address::getIpAddress() IPv4: %s\n", ipstr);
        return std::string(ipstr);
    }
    else
    {
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(value.addr.sin6.sin6_addr), ipstr, sizeof(ipstr));
        //coap_log_debug("Address::getIpAddress() IPv6: %s\n", ipstr);
        return std::string(ipstr);
    }
}

uint16_t Address::getPort() const noexcept
{
    auto value = m_Impl->m_rawAddr;
    if (value.size == sizeof(value.addr.sin))
    {
        return ntohs(value.addr.sin.sin_port);
    }
    else
    {
        return ntohs(value.addr.sin6.sin6_port);
    }
}

} // namespace CoapPlusPlus