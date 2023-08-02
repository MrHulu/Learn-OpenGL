/**
 * @file Option.h
 * @author Hulu
 * @brief libcoap库coap_opt_t的C++封装
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "coap/Information/OptionInformation.h"
#include <vector>
#include <cstdint>

typedef uint8_t coap_opt_t;
namespace CoapPlusPlus
{

class Option 
{
public:
    /**
     * @brief 构造一个Option对象
     * @details coap_opt_t的C++封装，存储了一个选项的信息。
     * 
     * @exception std::invalid_argument 传入的opt不是一个合法的选项
     */
    Option(Information::OptionNumber number, coap_opt_t* opt);

    /**
     * @brief 获取该选项的选项编号
     * 
     * @return 选项编号
     */
    Information::OptionNumber getNumber() const noexcept { return m_number; }

    /**
     * @brief 获取该选项的总长度，包括头部和数据，选项的头包含了选项的编号和长度
     * 
     * @return 从opt开始到选项结束之间的字节数。如果出现错误，该函数返回0，因为选项至少需要一个字节的存储空间。
    */
    size_t getSize() const noexcept;

    /**
     * @brief 获取该选项数据的长度
     * 
     * @return 选项数据长度，当前对象不是一个选项时返回0
     */
    size_t getLength() const noexcept;

    /**
     * @brief 获取该选项的值
     * 
     * @return 选择的值，当前对象不是一个选项时返回nullptr
     */
    const uint8_t* getValue() const noexcept;

    std::vector<uint8_t> getData() const noexcept {
        return std::vector<uint8_t>(getValue(), getValue() + getLength());
    }

private:
    Information::OptionNumber m_number;
    coap_opt_t* m_opt = nullptr;
};



};  // namespace CoapPlusPlus