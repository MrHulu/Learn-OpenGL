/**
 * @file ResourceInterface.h
 * @author Hulu
 * @brief 资源回应接口类定义
 * @version 0.1
 * @date 2023-06-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "coap/Information/PduInformation.h"
#include <string>

namespace CoapPlusPlus
{

class Resource;
class Session;
class ResponsePdu;
class RequestPdu;
class ResourceInterface
{
    friend class Resource;
private:
    void setResource(Resource* resource);
    Resource* r = nullptr;
protected:
    ResourceInterface(Information::RequestCode code) noexcept : m_code(code) {} 
public:
    virtual ~ResourceInterface() noexcept = default;

    /**
     * @brief 当收到请求时，资源自动调用该接口进行回应
     * 
     * @param session 会话信息
     * @param query 查询字符串
     * @param request 请求信息
     * @param response 回应信息
     */
    virtual void onRequest(Session session, std::string query, 
                        ResponsePdu response, RequestPdu request) = 0;

    /**
     * @brief 该资源回应接口对应的请求码
     * 
     * @return 请求码
     *      @retval Information::RequestCode::GET
     *      @retval Information::RequestCode::POST
     *      @retval Information::RequestCode::PUT
     *      @retval Information::RequestCode::DELETE
     * @see Information::RequestCode
     */
    Information::RequestCode code() const noexcept { return m_code; }

    /**
     * @brief 对资源的所有观察者发送观察包
     * 
     */
    void notifyObserver() noexcept;

private:
    Information::RequestCode m_code;
};


}  // namespace CoapPlusPlus