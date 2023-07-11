/**
 * @file Address.h
 * @author Hulu
 * @brief coap_address_t的C++封装
 * @version 0.1
 * @date 2023-07-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include <stdexcept>
#include <string>

namespace CoapPlusPlus
{


class Address {
public:
    Address(const sockaddr_in& address);
    Address(const sockaddr_in6& address);

    /**
     * @brief Construct a new Address object
     * 
     * @param ip ip地址
     * @param port 端口号
     * 
     * @exception std::invalid_argument 无效的IP地址
     */
    Address(const std::string& ip, uint16_t port);

    /**
     * @brief Construct a new Address object
     * 
     * @param address 
     * 
     * @exception std::invalid_argument 无效的coap_address_t地址
     */
    Address(const coap_address_t& address);

    Address(const Address& other) : m_rawAddr(other.m_rawAddr) {}
    Address(Address&& other) : m_rawAddr(std::move(other.m_rawAddr)) { other.m_rawAddr.size = 0; }
    ~Address() = default;

    Address& operator=(const Address& other) {
        if (this != &other) {
            m_rawAddr = other.m_rawAddr;
        }
        return *this;
    }
    Address& operator=(Address&& other) {
        if (this != &other) {
            m_rawAddr = std::move(other.m_rawAddr);
            other.m_rawAddr.size = 0;
        }
        return *this;
    }

    std::string getIpAddress() const;

    uint16_t getPort() const;

private:
    coap_address_t m_rawAddr;
};



} // namespace CoapPlusPlus
