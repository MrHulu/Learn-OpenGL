#pragma once
#include "Address.h"
#include <coap3/coap.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

namespace CoapPlusPlus
{

class Address::AddressImpl
{
public:
    AddressImpl(const sockaddr_in& address) noexcept
    {
        coap_address_t temp;
        coap_address_init(&temp);
        temp.size = sizeof(temp.addr.sin);
        temp.addr.sin = address;
        m_rawAddr = temp;
    }

    AddressImpl(const sockaddr_in6& address) noexcept
    {
        coap_address_t temp;
        coap_address_init(&temp);
        temp.size = sizeof(temp.addr.sin6);
        temp.addr.sin6 = address;
        m_rawAddr = temp;
    }

    AddressImpl(const std::string& ip, uint16_t port)
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

    AddressImpl(const coap_address_t& address) 
    {
        if (!(address.size == sizeof(address.addr.sin) || address.size == sizeof(address.addr.sin6)))
        {
            throw std::invalid_argument("Unknown address type");
        }
        m_rawAddr = address;
    }

    ~AddressImpl() {}

    // 拷贝构造函数
    // AddressImpl(const AddressImpl& other) { }
    
    // // 移动构造函数
    // AddressImpl(AddressImpl&& other);
    
    // // 拷贝赋值运算符
    // AddressImpl& operator=(const AddressImpl& other);
    
    // // 移动赋值运算符
    // AddressImpl& operator=(AddressImpl&& other);
    coap_address_t m_rawAddr;
};

} // end of namespace CoapPlusPlus