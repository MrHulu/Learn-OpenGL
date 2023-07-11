#include "Address.h"

namespace CoapPlusPlus
{

Address::Address(const sockaddr_in &address) noexcept
{
    m_rawAddr.size = sizeof(m_rawAddr.addr.sin);
    m_rawAddr.addr.sin = address;
}

Address::Address(const sockaddr_in6 &address) noexcept
{
    m_rawAddr.size = sizeof(m_rawAddr.addr.sin6);
    m_rawAddr.addr.sin6 = address;
}

Address::Address(const std::string &ip, uint16_t port)
{
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
    auto error = getaddrinfo(ip.c_str(), NULL, &hints, &res);
    if (error != 0) {
        std::string message = std::string("Invalid IP address: ") + gai_strerror(error);
        coap_log_debug("无法构造Address(%s, %d)对象，%s\n",ip.c_str() ,port ,message.c_str());
        throw std::invalid_argument(message.c_str());
    }

    if (res->ai_family == AF_INET) {
        m_rawAddr.size = sizeof(m_rawAddr.addr.sin);
        m_rawAddr.addr.sin = *((struct sockaddr_in*) res->ai_addr);
        m_rawAddr.addr.sin.sin_port = htons(port);
    } else if (res->ai_family == AF_INET6) {
        m_rawAddr.size = sizeof(m_rawAddr.addr.sin6);
        m_rawAddr.addr.sin6 = *((struct sockaddr_in6*) res->ai_addr);
        m_rawAddr.addr.sin6.sin6_port = htons(port);
    } else {
        freeaddrinfo(res);
        throw std::invalid_argument("Invalid IP address");
    }
    freeaddrinfo(res);
}

Address::Address(const coap_address_t &address)
{
    m_rawAddr = address;
    if (!(m_rawAddr.size == sizeof(m_rawAddr.addr.sin) || m_rawAddr.size == sizeof(m_rawAddr.addr.sin6)))
    {
        throw std::invalid_argument("Unknown address type");
    }
}

std::string Address::getIpAddress() const noexcept
{

    if (m_rawAddr.size == sizeof(m_rawAddr.addr.sin))
    {
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(m_rawAddr.addr.sin.sin_addr), ipstr, sizeof(ipstr));
        //coap_log_debug("Address::getIpAddress() IPv4: %s\n", ipstr);
        return std::string(ipstr);
    }
    else
    {
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(m_rawAddr.addr.sin6.sin6_addr), ipstr, sizeof(ipstr));
        //coap_log_debug("Address::getIpAddress() IPv6: %s\n", ipstr);
        return std::string(ipstr);
    }
}

uint16_t Address::getPort() const noexcept
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