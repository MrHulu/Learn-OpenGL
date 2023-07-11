#include "Address.h"

namespace CoapPlusPlus
{

Address::Address(const sockaddr_in &address)
{
    m_rawAddr.size = sizeof(m_rawAddr.addr.sin);
    m_rawAddr.addr.sin = address;
}

Address::Address(const sockaddr_in6 &address)
{
    m_rawAddr.size = sizeof(m_rawAddr.addr.sin6);
    m_rawAddr.addr.sin6 = address;
}

Address::Address(const std::string &ip, uint16_t port)
{
    if (inet_pton(AF_INET, ip.c_str(), &(m_rawAddr.addr.sin.sin_addr)) == 1)
    {
        m_rawAddr.size = sizeof(m_rawAddr.addr.sin);
        m_rawAddr.addr.sin.sin_family = AF_INET;
        m_rawAddr.addr.sin.sin_port = htons(port);
    }
    else if (inet_pton(AF_INET6, ip.c_str(), &(m_rawAddr.addr.sin6.sin6_addr)) == 1)
    {
        m_rawAddr.size = sizeof(m_rawAddr.addr.sin6);
        m_rawAddr.addr.sin6.sin6_family = AF_INET6;
        m_rawAddr.addr.sin6.sin6_port = htons(port);
    }
    else
    {
        throw std::invalid_argument("Invalid IP address");
    }
}

Address::Address(const coap_address_t &address) : m_rawAddr(address)
{
    if (!(m_rawAddr.size == sizeof(m_rawAddr.addr.sin) || m_rawAddr.size == sizeof(m_rawAddr.addr.sin6)))
    {
        throw std::invalid_argument("Unknown address type");
    }
}

std::string Address::getIpAddress() const
{
    char ipstr[INET6_ADDRSTRLEN];

    if (m_rawAddr.size == sizeof(m_rawAddr.addr.sin6))
    {
        inet_ntop(AF_INET6, &(m_rawAddr.addr.sin6.sin6_addr), ipstr, sizeof(ipstr));
    }
    else
    {
        inet_ntop(AF_INET, &(m_rawAddr.addr.sin.sin_addr), ipstr, sizeof(ipstr));
    }
    return std::string(ipstr);
}

uint16_t Address::getPort() const
{
    if (m_rawAddr.size == sizeof(m_rawAddr.addr.sin))
    {
        return ntohs(m_rawAddr.addr.sin.sin_port);
    }
    else
    {
        return ntohs(m_rawAddr.addr.sin6.sin6_port);
    }
}

} // namespace CoapPlusPlus