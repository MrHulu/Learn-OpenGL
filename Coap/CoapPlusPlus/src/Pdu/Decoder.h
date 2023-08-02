/**
 * @file Decoder.h
 * @author Hulu
 * @brief Decoder for decoding CoAP options.
 * @version 0.1
 * @date 2023-06-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <vector>
#include <cstdint>

namespace CoapPlusPlus
{

class Decoder
{
public:
    /**
     * @brief 构造一个Decoder对象，用于解码CoAP选项的值
     * 
     */
    Decoder() = default;
    ~Decoder() = default;

    /**
     * @brief 对多长度的字节序列进行解码，返回最多4字节的解码值
     * 
     * @param data 需要解码的数据
     * @return uint32_t 解码后的值
     */
    static uint32_t Decode(std::vector<uint8_t> data) noexcept;
    
    /**
     * @brief 对多长度的字节序列进行解码，返回最多8字节的解码值
     * 
     * @param data 需要解码的数据
     * @return uint64_t 解码后的值
     */
    static uint64_t Decode8(std::vector<uint8_t> data) noexcept;

};



};  // namespace CoapPlusPlus