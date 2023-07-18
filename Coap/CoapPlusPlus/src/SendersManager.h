/**
 * @file SendersManager.cc
 * @author Hulu
 * @brief 客户端发送器管理器
 * @version 0.3
 * @date 2023-07-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once


#include <coap3/coap.h>
#include "coap/Information/PduInformation.h"
#include <map>
#include <memory>

namespace CoapPlusPlus
{

class RequestPdu;
class BinaryConstView;
class BinaryConst;
class Handling;
class SendersManager
{
public:
    SendersManager(coap_session_t& coap_session);
    ~SendersManager();

    /**
     * @brief 向指定的对等设备发送CoAP消息。
     * 
     * @param pdu 请求
     * @param handling 回应的处理器, 调用该函数后handling的生命周期由SendersManager管理。
     *                 如果传入nullptr则会先遍历处理器列表是否存在与pdu中token对应的处理器，没有再调用默认的处理器进行响应处理
     * @see updateDefaultHandling(std::unique_ptr<Handling>handling);
     * 
     * @retval true 发送成功
     * @retval false 发送失败，handling会被立马销毁
     * 
     * @exception AlreadyExistException 已经存在相同token的处理器。
     *            如果更新处理器请使用removeHandling()函数删除旧的处理器，
     *            如果使用旧的处理器，传入nullptr即可。
     */
    bool send(RequestPdu pdu, std::unique_ptr<Handling> handling);

    /**
     * @brief 更新默认的响应处理器，当send函数中没有指定处理器时或者传入nullptr是，内部使用默认的处理器
     * 
     * @code {.cpp}
     * class DefaultHandling : public Handling 
     * {
     * DefaultHandling(BinaryConst token) : Handling(COAP_INVALID_MID, token) { }
     * 
     *  ... other code ...
     * 
     * bool onAck(Session& session, const RequestPdu* request, const ResponsePdu* response) noexcept override {
     *  Pdu::LogPdu(COAP_LOG_INFO, request);
     *  Pdu::LogPdu(COAP_LOG_INFO, response);
     *  return true;
     * }
     * 
     * void onNAck(Session& session, RequestPdu request, NAckReason reason) noexcept override { }
     * 
     * void readyDestroyed() noexcept override { }
     * 
     * bool isFinished() noexcept override { return false; }
     * 
     * ... other code ...
     * 
     * };
     * @endcode
     * 
     * @note 默认的处理器会在send函数中传入nullptr时使用，默认的处理器的isFinished()接口不会生效，所以调用此函数后，上一个默认处理器才会被销毁。
     */
    void updateDefaultHandling(std::unique_ptr<Handling>handling) noexcept;

    /**
     * @brief 创建一个新的token。token用于请求和响应的匹配  
     * @see RequestPdu
     * 
     * @return 新的token令牌 @see BinaryConst
     * @exception InternalException 创建token失败，内部错误
     */
    BinaryConst createToken() const;

    /**
     * @brief 创建一个带有token的新的请求
     * 
     * @param type 消息类型
     * @param code 请求码
     * 
     * @return 一个请求 @see RequestPdu @see Pdu
     * @exception InternalException 创建请求失败，内部错误
     * 
     * @note 创建的请求的payload和option为空, 需要调用者设置
     */
    RequestPdu createRequest(Information::MessageType type, Information::RequestCode code) const;

    /**
     * @brief 获取一个处理器
     * 
     * @param token 
     * @return std::shared_ptr<Handling> 
     * 
     * @exception TargetNotFoundException 未找到对应的处理器
     */
    std::shared_ptr<Handling> getHandling(const BinaryConstView& token) const;

    /**
     * @brief 移除一个处理器
     * 
     * @param token 
     * @retval true 移除成功
     * @retval false 移除失败，未找到对应的处理器
     */
    bool removeHandling(const BinaryConstView& token) noexcept;

private:
    void registerHandlerInit() noexcept;
    static void NackHandler(coap_session_t *session, const coap_pdu_t *sent, coap_nack_reason_t reason, const coap_mid_t mid);
    static coap_response_t AckHandler(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, const coap_mid_t mid);

private:
    coap_session_t *m_coap_session = nullptr;
    std::map<BinaryConst, Handling*> m_handlings; 
    class DefaultHandling;
    Handling* m_defaultHandling = nullptr;
};


} // namespace CoapPlusPlus