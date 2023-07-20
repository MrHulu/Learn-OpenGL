/**
 * @file BinaryConst.h
 * @author Hulu
 * @brief BinaryConst类定义
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include <span>
#include <string>

namespace CoapPlusPlus {

class BinaryConst
{
    friend class BinaryConstView;
    BinaryConst(coap_bin_const_t* raw, bool owned);
public:
    BinaryConst(const BinaryConst& other) {
        if(this != &other) {
            BinaryConst copy = DeepCopy(other.m_rawData);
            std::swap(m_rawData, copy.m_rawData);
            std::swap(m_owned, copy.m_owned);
        }
    }
    BinaryConst(BinaryConst&& other) noexcept : m_rawData(other.m_rawData), m_owned(other.m_owned) {
        if(this != &other) {
            other.m_rawData = nullptr;
            other.m_owned = false;
        }
    }
    BinaryConst& operator=(const BinaryConst& other) {
        if (this != &other) {
            BinaryConst copy = DeepCopy(other.m_rawData);
            std::swap(m_rawData, copy.m_rawData);
            std::swap(m_owned, copy.m_owned);
        }
        return *this;
    }

    BinaryConst& operator=(BinaryConst&& other) noexcept {
        if (this != &other) {
            std::swap(m_rawData, other.m_rawData);
            std::swap(m_owned, other.m_owned);
        }
        return *this;
    }
    
    bool operator<(const BinaryConst& other) const {
        if(m_rawData != nullptr && other.m_rawData != nullptr) {
            if (m_rawData->length < other.m_rawData->length) 
                return true; 
            else if (m_rawData->length > other.m_rawData->length)
                return false;
            else
                return std::memcmp(m_rawData->s, other.m_rawData->s, m_rawData->length) < 0;
        }else{
            return false;
        }
    }

    bool operator== (const BinaryConst &other) const noexcept {
        return coap_binary_equal(m_rawData, other.m_rawData);
    }

    
    ~BinaryConst();
    /**
     * @brief 获得一个引用coap_bin_const_t结构体的BinaryConst
     * 
     * @param raw coap_bin_const_t结构体指针
     * 
     * @exception std::invalid_argument 当raw为空时抛出
     * @return BinaryConst
     */
    static BinaryConst Reference(const coap_bin_const_t* raw);

    /**
     * @brief 深拷贝一个coap_bin_const_t结构体的BinaryConst
     * 
     * @param raw coap_bin_const_t结构体指针
     * 
     * @exception std::invalid_argument 当raw为空时抛出
     * @exception InternalException 无法分配内存时抛出
     * @return BinaryConst 
     */
    static BinaryConst DeepCopy(coap_bin_const_t* raw);

    /**
     * @brief 创建一个BinaryConst
     * 
     * @param size 要分配给字节数据的大小。
     * @param data 要放在新的BinaryConst对象中的数据。
     * 
     * @exception std::invalid_argument 当为raw为空时抛出
     * @exception InternalException 无法分配内存时抛出
     * @return BinaryConst 
     */
    static BinaryConst Create(size_t size, const uint8_t* data);

    /**
     * @brief 获得BinaryConst对象中的数据字节大小
     * 
     * @exception DataWasReleasedException 当对象中的字节数据已经被释放时抛出
     * @return size_t 
     */
    size_t size() const;

    /**
     * @brief 获得BinaryConst对象中的字节数据
     * 
     * @exception DataWasReleasedException 当对象中的字节数据已经被释放时抛出
     * @return std::span<uint8_t>类型的数据 
     */
    std::span<uint8_t> data() const;

        /**
     * @brief 获得对象中引用的字节数据
     * 
     * @exception DataWasReleasedException 当对象中的字节数据已经被释放时抛出
     * @return uint64_t 
     */
    uint64_t toUInt64() const;

    /**
     * @brief 获得对象中引用的字节数据
     * 
     * @exception DataWasReleasedException 当对象中的字节数据已经被释放时抛出
     * @return 十六进制字符串 
     */
    std::string toHexString() const;

private:
    const coap_bin_const_t* rawData() const { return m_rawData; }

private: 
    coap_bin_const_t* m_rawData = nullptr;
    bool m_owned = false;
};


};// namespace CoapPlusPlus PlusPlus