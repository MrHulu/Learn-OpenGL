/**
 * @file Pdu.h
 * @author Hulu
 * @brief libcoap库coap_pdu_t的C++封装
 * @version 0.1
 * @date 2023-06-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "coap/Information/PduInformation.h"
#include "coap/Information/OptionInformation.h"
#include "coap/Log.h"
#include <vector>

typedef int coap_mid_t;
struct coap_pdu_t;
namespace CoapPlusPlus {
using namespace Information;
using Mid = coap_mid_t;

class Option;
class Options;
class OptFilter;
class Payload;

/**
 * @brief CoAP PDU的形式是 --header--|--optional token--|--optional options--|--optional payload--
 * @details 通常对于客户端来说，在创建一个请求时，需要先创建PDU，然后再填充相应的信息。
 *          通常对于服务器来说，在调用响应处理程序之前，已经创建了响应PDU，
 *          其中已经加入了可选的令牌，响应PDU需要从可选的选项开始适当地更新。
 *          注意，更新响应PDU的代码变量将导致响应PDU被传送。
 *          如果代码没有被更新，并且PDU是CONFIRMABLE类型的，那么响应PDU将作为一个空的ACK包被传送。
 *          响应PDU总是被底层库释放掉。
 * 
 * @note PDU必须按照正确的顺序，从左到右构建。例如，如果您想要添加payload，您必添加完选项。
 * 
 */
class Pdu
{
    friend class SendersManager;
public:
    /**
     * @brief 记录打印Pdu的信息。
     *        格式为：v:version t:messageType c:code i:messageId {token} [ option ] :: payload
     * 
     * @param level log的级别
     * @param pdu pdu对象
     */
    static void LogPdu(LOG_LEVEL level, const Pdu* pdu) noexcept;

    virtual Payload payload() const noexcept = 0;

    virtual bool setPayload(Payload payload) noexcept = 0;

    /**
     * @brief 获取该对象的Pdu的类型，默认为Confirmable
     * 
     * @return 返回该对象的Pdu的类型
     *      @retval Confirmable 需要应答的消息
     *      @retval NonConfirmable 无需应答的消息
     *      @retval Acknowledgement 该消息是一个应答
     *      @retval Reset 该消息是一个重置消息
     */
    MessageType messageType() const noexcept { return m_type; }

    /**
     * @brief 设置该对象的Pdu的类型
     * 
     * @param type Pdu的类型
     * @return 设置是否成功, 如果该对象的Pdu已经被释放，则设置失败
     */
    bool setMessageType(MessageType type) noexcept;

    /**
     * @brief 获取该对象的Pdu的messageId
     * @details MID用于标识消息，确保消息的顺序和避免重复。
     *          每个CoAP消息（请求或响应）都有一个唯一的MID。
     *          MID的主要目的是在传输层处理重复消息和消息丢失，而不是在应用层关联请求和响应
     * 
     * @return 返回该对象的Pdu的messageId
     * 
     * @exception DataWasReleasedException 当该对象的Pdu已经被释放时抛出此异常
     */
    Mid messageId() const;

    /**
     * @brief 判断该对象的Pdu是否包含某个选项
     * 
     * @param number 需要判断的选项的编号
     * @return 选项是否存在
     * 
     * @exception DataWasReleasedException 当该对象的Pdu已经被释放时抛出此异常
     */
    bool isContainOption(OptionNumber number) const;

    /**
     * @brief 获取该对象的Pdu的所有选项，可以通过OptFilter过滤选项
     * 
     * @param filter 过滤器, 可以过滤一些选项
     * @return 返回该对象的Pdu过滤后的所有选项
     * 
     * @exception DataWasReleasedException 当该对象的Pdu已经被释放时抛出此异常
     */
    std::vector<Option> getOptions(OptFilter filter) const;
    std::vector<Option> getOptions() const;

    /**
     * @brief 为该对象的Pdu添加一个选项列表，大多数情况下用于创建请求
     * 
     * @param options 选项列表
     * @return 添加是否成功
     *      @retval true Pdu成功添加了选项
     *      @retval false 如果Pdu添加了payload则添无法再添加选项或者options参数里面的选项列表为空
     * 
     * @exception DataWasReleasedException 当该对象的Pdu已经被释放时抛出此异常
     * @attention 如果某个选项已经存在，将不会覆盖，只会跳过
     * @note 选项无需按照正确的数字选项顺序添加，
     *            虽然它们在PDU中使用与前一个选项编号的相对数字偏移来存储。但是内部会自动排序
     */
    bool addOptions(Options options);

protected:
    /**
     * @brief Construct a new Pdu object
     * 
     * @param pdu 
     * @exception invalid_argument pdu is nullptr
     */
    Pdu(coap_pdu_t* pdu);
    ~Pdu() noexcept { }
    coap_pdu_t* getPdu() const noexcept { return m_rawPdu; }

protected:
    coap_pdu_t* m_rawPdu = nullptr;
    
private:
    MessageType m_type = MessageType::Confirmable;
};


}; // namespace CoapPlusPlus