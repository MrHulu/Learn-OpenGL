/**
 * @file BinaryConstView.h
 * @author Hulu
 * @brief BinaryConstView类定义
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include "coap/exception.h"
#include "BinaryConst.h"

namespace CoapPlusPlus {

class BinaryConstView
{
public:
    BinaryConstView(const BinaryConstView& other) : BinaryConstView(other.m_rawData) { }
    BinaryConstView(BinaryConstView&& other) noexcept  : m_rawData(other.m_rawData){ }
    /**
     * @brief 返回一个新的二进制对象，该对象引用给定的数据。
     * 
     * @param raw libcoap库的二进制数据结构指针
     * @exception std::invalid_argument 当raw为空时抛出
     */
    BinaryConstView(const coap_bin_const_t* raw) 
        : m_rawData(raw)  { }
    BinaryConstView(const BinaryConst& binary) 
        : m_rawData(binary.rawData())  { }

    ~BinaryConstView() noexcept = default;

    bool operator== (const BinaryConstView &other) const noexcept {
        return coap_binary_equal(m_rawData, other.m_rawData);
    }

    /**
     * @brief 转换为BinaryConst对象
     * 
     * @return 一个BinaryConst对象 
     */
    BinaryConst toBinaryConst() const { 
        return BinaryConst::DeepCopy(const_cast<coap_bin_const_t*>(m_rawData)); 
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
    const coap_bin_const_t* m_rawData;
};


};// namespace CoapPlusPlus
