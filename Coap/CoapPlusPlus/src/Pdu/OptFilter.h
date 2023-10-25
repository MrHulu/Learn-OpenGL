/**
 * @file OptFilter.h
 * @author Hulu
 * @brief libcoap库coap_opt_filter_t的C++封装
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "coap/Information/OptionInformation.h"

#include <vector>

struct coap_opt_filter_t;
namespace CoapPlusPlus
{

class OptFilter 
{
    friend class Pdu;
public:
    /**
     * @brief 构造一个OptFilter对象
     * @details coap_opt_filter_t的C++封装，Option的过滤器。
     * 
     * @param optNumbers 选项编号列表
     */
    OptFilter(std::vector<Information::OptionNumber> optNumbers);

    /**
     * @brief 判断该过滤器是否包含某个选项
     * 
     * @param number 是否包含的选项编号
     * @return true 选项被包含
     * @return false 选项未被包含
     */
    bool isContain(Information::OptionNumber number) noexcept;

    /**
     * @brief 向过滤器中添加一个选项
     * 
     * @param number 需要添加的选项编号
     * @return 操作是否成功
     */
    bool insert(Information::OptionNumber number) noexcept;

    /**
     * @brief 从过滤器中移除一个选项    
     * 
     * @param number 需求移除的选项编号
     * @return 操作是否成功
     */
    bool remove(Information::OptionNumber number) noexcept;

private: 
    const coap_opt_filter_t* getOptFilter() const noexcept;

};


}  // namespace CoapPlusPlus