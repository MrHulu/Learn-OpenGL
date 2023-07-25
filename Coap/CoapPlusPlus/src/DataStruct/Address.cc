#include <coap3/coap.h>
#include "Address.h"

namespace CoapPlusPlus
{

Address::Address(const sockaddr_in &address) noexcept
{
    coap_address_t temp;
    coap_address_init(&temp);
    temp.size = sizeof(temp.addr.sin);
    temp.addr.sin = address;
    m_rawAddr = temp;
}

Address::Address(const sockaddr_in6 &address) noexcept
{
    coap_address_t temp;
    coap_address_init(&temp);
    temp.size = sizeof(temp.addr.sin6);
    temp.addr.sin6 = address;
    m_rawAddr = temp;
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
        coap_log_warn("无法构造Address(%s, %d)对象，%s\n",ip.c_str() ,port ,message.c_str());
        throw std::invalid_argument(message.c_str());
    }
    coap_address_t temp;
    coap_address_init(&temp);
    if (res->ai_family == AF_INET) {
        temp.size = sizeof(temp.addr.sin);
        temp.addr.sin = *((struct sockaddr_in*) res->ai_addr);
        temp.addr.sin.sin_port = htons(port);
    } else if (res->ai_family == AF_INET6) {
        temp.size = sizeof(temp.addr.sin6);
        temp.addr.sin6 = *((struct sockaddr_in6*) res->ai_addr);
        temp.addr.sin6.sin6_port = htons(port);
    } else {
        freeaddrinfo(res);
        throw std::invalid_argument("Invalid IP address");
    }
    m_rawAddr = temp;
    freeaddrinfo(res);
}

Address::Address(const coap_address_t &address)
{
    if (!(address.size == sizeof(address.addr.sin) || address.size == sizeof(address.addr.sin6)))
    {
        throw std::invalid_argument("Unknown address type");
    }
    m_rawAddr = address;
}

Address::Address(Address &&other) : m_rawAddr(std::move(other.m_rawAddr)) 
{ 
    coap_address_t temp;
    coap_address_init(&temp);
    temp.size = 0; 
    other.m_rawAddr = temp;
}

Address &Address::operator=(const Address &other)
{
    if (this != &other) {
        m_rawAddr = other.m_rawAddr;
    }
    return *this;
}

Address &Address::operator=(Address &&other)
{
    if (this != &other) {
        m_rawAddr = std::move(other.m_rawAddr);
        coap_address_t temp;
        coap_address_init(&temp);
        temp.size = 0; 
        other.m_rawAddr = temp;
    }
    return *this;
}

bool Address::operator==(const Address &other) const noexcept
{
    auto temp = std::get<coap_address_t>(m_rawAddr);
    auto otherTemp = std::get<coap_address_t>(other.m_rawAddr);
    return temp.size == otherTemp.size && 
            memcmp(&temp.addr, &otherTemp.addr, temp.size) == 0;
}

std::string Address::getIpAddress() const noexcept
{
    auto value = std::get<coap_address_t>(m_rawAddr);
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
    auto value = std::get<coap_address_t>(m_rawAddr);
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