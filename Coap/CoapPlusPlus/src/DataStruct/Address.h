/**
 * @file Address.h
 * @author Hulu
 * @brief coap_address_t的C++封装
 * @version 0.2
 * @date 2023-07-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <stdexcept>
#include <string>
#include <variant>

struct coap_address_t;
struct sockaddr_in;
struct sockaddr_in6;
namespace CoapPlusPlus
{


class Address {
public:
    Address(const sockaddr_in& address) noexcept;
    Address(const sockaddr_in6& address) noexcept;

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
    Address(Address&& other);
    ~Address() = default;

    Address& operator=(const Address& other);
    Address& operator=(Address&& other);
    bool operator==(const Address& other) const noexcept;

    /**
     * @brief 获得ip地址
     * 
     * @return std::string 
     */
    std::string getIpAddress() const noexcept;

    /**
     * @brief 获得端口号
     * 
     * @return uint16_t 
     */
    uint16_t getPort() const noexcept;

private:
    std::variant<coap_address_t> m_rawAddr;
};



} // namespace CoapPlusPlus
