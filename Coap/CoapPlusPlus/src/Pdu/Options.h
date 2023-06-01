/**
 * @file Options.h
 * @author Hulu
 * @brief libcoap库coap_optList_t的C++封装
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include "coap/Information/OptionInformation.h"
#include <vector>

namespace CoapPlusPlus
{

class Options
{
public:
    /**
     * @brief 构造一个Options对象
     * @details coap_optlist_t的C++封装，用于存储CoAP消息的选项。
     * 
     * @exception invalid_argument 如果optList为nullptr，会抛出该异常
     */
    Options(coap_optlist_t* optList);

    /**
     * @brief 插入一个选项
     * @details 该函数会在optList中插入一个选项，如果该选项已经存在，则会覆盖原有的选项。
     *          在可能的情况下，选项数据值需要去掉前导零（big endian），以减少PDU中需要的数据量。
     *          在某些情况下，如果不去掉前导零，就会超过一个选项的最大数据量，从而成为非法。
     *          这可以通过使用Option::EncodeVarSafe或者Option::EncodeVarSafe8完成。
     * 
     * @param number 需要插入的选项的编号
     * @param data 需要插入的选项的数据
     * @return 是否插入成功
     */
    bool insert(Information::OptionNumber number, std::vector<uint8_t> data) noexcept;

private:
    coap_optlist_t* getOptList() const noexcept { return m_optList; }

private:
    coap_optlist_t* m_optList = nullptr;
};




};  // namespace CoapPlusPlus