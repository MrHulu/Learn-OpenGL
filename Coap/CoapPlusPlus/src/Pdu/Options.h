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
#include <string>

namespace CoapPlusPlus
{

class Options
{
    friend class Pdu;
public:
    /**
     * @brief 构造一个Options对象
     * @details coap_optlist_t的C++封装，用于存储CoAP消息的选项。
     * 
     */
    Options(coap_optlist_t* optList);

    /**
     * @brief 插入一个选项
     * @details 该函数会在optList中插入一个选项，如果该选项已经存在，则会覆盖原有的选项。
     *          在可能的情况下，选项数据值需要去掉前导零（big endian），以减少PDU中需要的数据量。
     *          在某些情况下，如果不去掉前导零，就会超过一个选项的最大数据量，从而成为非法。
     * @note 该函数不会检查选项的数据是否合法，如果需要检查，可以使用 Decoder 或者 Encoder。
     * 
     * @param number 需要插入的选项的编号
     * @param data 需要插入的选项的数据
     * @return 是否插入成功
     */
    bool insert(Information::OptionNumber number, std::vector<uint8_t> data) noexcept;

    /**
     * @brief 插入一个URI选项
     * @details 任何路径或查询都会被分解成单独的分段路径或查询选项然后和端口选项被插入到该对象。
     * 
     * @param path URI字符串，例如："coap://[::1]:40288/coapcpp/pdu/option?a=1&b=2"
     * @return 是否插入成功
     * 
     * @exception std::invalid_argument 传入的path不是一个合法的URI  
     */
    bool insertURIOption(std::string path);

    /**
     * @brief 插入一个观察选项
     * 
     * @param enable @c true 表示启用观察选项， @c false 表示取消观察选项
     * @return 是否插入成功  
     */
    bool insertOsberveOption(bool enable) noexcept;

    /**
     * @brief 插入一个内容格式选项
     * 
     * @param format @enum Information::ContentFormatType
     * @return 是否插入成功   
     */
    bool insertContentFormatOption(Information::ContentFormatType format) noexcept;


private:
    coap_optlist_t* getOptList() const noexcept { return m_optList; }
    void deleteOptList() noexcept { coap_delete_optlist(m_optList); }

private:
    coap_optlist_t* m_optList = nullptr;
};




};  // namespace CoapPlusPlus