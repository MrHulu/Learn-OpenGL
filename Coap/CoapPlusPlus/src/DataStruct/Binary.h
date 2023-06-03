/**
 * @file Binary.h
 * @author Hulu
 * @brief Binary类定义
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include <span>

namespace CoapPlusPlus {

class Binary
{
    Binary(coap_binary_t* raw, bool owned);
    Binary(const Binary& other) : Binary(DeepCopy(other.m_rawData).m_rawData, true) {}
    Binary(Binary&& other) noexcept : m_rawData(other.m_rawData), m_owned(other.m_owned) {
        other.m_rawData = nullptr;
        other.m_owned = false;
    }
    Binary& operator=(const Binary& other) {
        if (this != &other) {
            Binary copy = DeepCopy(other.m_rawData);
            std::swap(m_rawData, copy.m_rawData);
            std::swap(m_owned, copy.m_owned);
        }
        return *this;
    }

    Binary& operator=(Binary&& other) noexcept {
        if (this != &other) {
            std::swap(m_rawData, other.m_rawData);
            std::swap(m_owned, other.m_owned);
        }
        return *this;
    }
public:
    
    ~Binary();
    /**
     * @brief 获得一个引用coap_binary_t结构体的Binary
     * 
     * @param raw coap_binary_t结构体指针
     * 
     * @exception std::invalid_argument 当raw为空时抛出
     * @return Binary
     */
    static Binary Reference(const coap_binary_t* raw);

    /**
     * @brief 深拷贝一个coap_binary_t结构体的Binary
     * 
     * @param raw coap_binary_t结构体指针
     * 
     * @exception std::invalid_argument 当raw为空时抛出
     * @exception CallCoapLibFuncException 无法分配内存时抛出
     * @return Binary 
     */
    static Binary DeepCopy(coap_binary_t* raw);

    /**
     * @brief 创建一个Binary
     * 
     * @param size 要分配给字节数据的大小。
     * @param data 要放在新的Binary对象中的数据。
     * 
     * @exception std::invalid_argument 当为raw为空时抛出
     * @exception CallCoapLibFuncException 无法分配内存时抛出
     * @return Binary 
     */
    static Binary Create(size_t size, uint8_t* data);

    /**
     * @brief 获得Binary对象中的数据字节大小
     * 
     * @exception DataWasReleasedException 当对象中的字节数据已经被释放时抛出
     * @return size_t 
     */
    size_t size() const;

    /**
     * @brief 获得Binary对象中的字节数据
     * 
     * @exception DataWasReleasedException 当对象中的字节数据已经被释放时抛出
     * @return std::span<uint8_t>类型的数据 
     */
    std::span<uint8_t> data() const;

private: 
    coap_binary_t* m_rawData;
    bool m_owned;
};


};// namespace CoapPlusPlus PlusPlus