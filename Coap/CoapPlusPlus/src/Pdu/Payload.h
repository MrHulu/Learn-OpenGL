/**
 * @file Payload.h
 * @author Hulu
 * @brief Pdu消息中的Payload部分
 * @version 0.1
 * @date 2023-06-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "coap/Information/OptionInformation.h"
#include <stdint.h>
#include <span>
#include <algorithm>

namespace CoapPlusPlus
{

class Payload
{
public:
    Payload(int size, const uint8_t *data, Information::ContentFormatType type) noexcept {
        m_type = type;
        m_data = std::span<uint8_t>(const_cast<uint8_t*>(data), size);
    }

    Payload() noexcept : m_type(Information::ContentFormatType::None) {
        m_data = std::span<uint8_t>();
    }

    // 默认浅拷贝就可以了
    // Payload(const Payload &other) noexcept {
    //     m_type = other.m_type;
    //     m_data = other.m_data;
    // }

    ~Payload() noexcept = default;

    bool operator== (const Payload &other) const noexcept {
        return m_type == other.m_type && std::ranges::equal(m_data, other.m_data);
    }

    /**
     * @brief 获得payload中的数据字节大小
     * 
     * @return size_t 
     */
    size_t size() const noexcept { return m_data.size(); }

    /**
     * @brief 获得payload中的字节数据
     * 
     * @return std::span<uint8_t>类型的数据 
     */
    std::span<uint8_t> data() const noexcept { return m_data; }

    /**
     * @brief 获得payload中的数据类型
     * 
     * @return Information::ContentFormatType 数据流类型 @enum Information::ContentFormatType
     */
    Information::ContentFormatType type() const noexcept { return m_type; }

private:
    Information::ContentFormatType m_type;
    std::span<uint8_t> m_data;
};



}; // namespace CoapPlusPlus