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

#include <coap3/coap.h>
#include "coap/Information/OptionInformation.h"

namespace CoapPlusPlus
{

class Option 
{
public:
    /**
     * @brief 构造一个Option对象
     * @details coap_opt_t的C++封装，存储了一个选项的信息。
     * 
     * @attention 该构造函数不会检查opt是否非法
     */
    Option(Information::OptionNumber number, coap_opt_t* opt);

    /**
     * @brief 获取该选项的选项编号
     * 
     * @return 选项编号
     */
    Information::OptionNumber getNumber() const noexcept { return m_number; }

    /**
     * @brief 获取该选项的长度
     * 
     * @return 选项长度，当前对象不是一个选项时返回0
     */
    size_t getLength() const noexcept { return coap_opt_length(m_opt); }

    /**
     * @brief 获取该选项的值
     * 
     * @return 选择的值，当前对象不是一个选项时返回nullptr
     */
    const uint8_t* getValue() const noexcept { return coap_opt_value(m_opt); }

private:
    Information::OptionNumber m_number;
    coap_opt_t* m_opt = nullptr;
};



};  // namespace CoapPlusPlus