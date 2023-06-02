/**
 * @file Encoder.h
 * @author Hulu
 * @brief Encoder for endoding CoAP options.
 * @version 0.1
 * @date 2023-06-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include <vector>
#include <stdexcept>

namespace CoapPlusPlus
{

class Encoder
{
public:
    /**
     * @brief 构造一个Encoder对象，用于编码CoAP选项的值
     * 
     * @param value 需要编码的值
     * 
     * @exception invalid_argument 如果value非法，会抛出该异常
     */
    Encoder(uint32_t value) {
        uint8_t buf[4] = {0};
        m_data.clear();
        m_length = coap_encode_var_safe(buf, sizeof(buf), value);
        if(m_length != 0 && m_length <= sizeof(buf)) {
            m_data.reserve(m_length);
            m_data.insert(m_data.end(), buf, buf + m_length);
        }else if(m_length == 0 && m_length <= sizeof(buf)) {

        }else {
            throw std::invalid_argument("Invalid value for encoding.");
        }
    }
    
    /**
     * @brief 构造一个Encoder对象，用于编码CoAP选项的值
     * 
     * @param value 需要编码的值
     * 
     * @exception invalid_argument 如果value非法，会抛出该异常
     */
    Encoder(int32_t value): Encoder(static_cast<uint32_t>(value)) {}

    /**
     * @brief 构造一个Encoder对象，用于编码CoAP选项的值
     * 
     * @param value 需要编码的值
     * 
     * @exception invalid_argument 如果value非法，会抛出该异常
     */
    Encoder(uint64_t value) {
        uint8_t buf[8] = {0};
        m_data.clear();
        m_length = coap_encode_var_safe8(buf, sizeof(buf), value);
        if(m_length != 0 && m_length <= sizeof(buf)) {
            m_data.reserve(m_length);
            m_data.insert(m_data.end(), buf, buf + m_length);
        }else if(m_length == 0 && m_length <= sizeof(buf)) {

        }else {
            throw std::invalid_argument("Invalid value for encoding.");
        }
    }
    
    ~Encoder() noexcept = default;

    /**
     * @brief 得到用于编码值的字节数（当编码值为0时可以为0）
     * 
     * @return 编码值的字节数 
     */
    uint32_t getLength() const noexcept { return m_length; }

    /**
     * @brief 得到编码后的数据
     * 
     * @return std::vector<uint8_t>格式的编码后的数据 
     */
    std::vector<uint8_t> getData() const noexcept { return m_data; }

private:
    //Encoder(const Encoder&) = delete;
    //Encoder& operator=(const Encoder&) = delete;
    //Encoder(Encoder&&) = delete;
    //Encoder& operator=(Encoder&&) = delete;

private:
    uint32_t m_length = 0;
    std::vector<uint8_t> m_data;
};


};  // namespace CoapPlusPlus