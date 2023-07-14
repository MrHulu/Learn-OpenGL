/**
 * @file BinaryView.h
 * @author Hulu
 * @brief BinaryView类定义
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include "coap/exception.h"
#include "coap/DataStruct/Binary.h"

namespace CoapPlusPlus {

class BinaryView
{
public:
    /**
     * @brief 返回一个新的二进制对象，该对象引用给定的数据。
     * 
     * @param raw libcoap库的二进制数据结构指针
     * @exception std::invalid_argument 当raw为空时抛出
     */
    BinaryView(const coap_binary_t* raw) : m_rawData(raw) { }

    ~BinaryView() noexcept = default;

    bool operator<(const BinaryView& other) const {
        if (m_rawData->length < other.m_rawData->length) 
            return true; 
        else if (m_rawData->length > other.m_rawData->length)
            return false;
        else
            return std::memcmp(m_rawData->s, other.m_rawData->s, m_rawData->length) < 0;
    }

    /**
     * @brief 转换为Binary对象
     * 
     * @return 一个Binary对象 
     */
    Binary toBinary() const { 
        return Binary::DeepCopy(const_cast<coap_binary_t*>(m_rawData)); 
    }

    /**
     * @brief 获得对象中引用的数据字节大小
     * 
     * @exception DataWasReleasedException 当对象中的字节数据已经被释放时抛出
     * @return size_t 
     */
    size_t size() const { 
        if(m_rawData == nullptr)
            throw DataWasReleasedException("");
        return m_rawData->length; 
    }

    /**
     * @brief 获得对象中引用的字节数据
     * 
     * @exception DataWasReleasedException 当对象中的字节数据已经被释放时抛出
     * @return std::span<uint8_t>类型的数据 
     */
    std::span<uint8_t> data() const { 
        if(m_rawData == nullptr)
            throw DataWasReleasedException("");
        return std::span<uint8_t>(const_cast<uint8_t*>(m_rawData->s), m_rawData->length);
     }

private:
    const coap_binary_t* m_rawData;
};


};// namespace CoapPlusPlus
