/**
 * @file DefaultHandling.h
 * @author Hulu
 * @brief 默认的响应处理器
 * @version 0.1
 * @date 2023-07-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "coap/Handling.h"
#include "coap/SendersManager.h"

namespace CoapPlusPlus
{

class RequestPdu;
class ResponsePdu;
class SendersManager::DefaultHandling : public Handling {

public:
    DefaultHandling(BinaryConst token) noexcept;
    ~DefaultHandling() noexcept override {}

    bool onAck(Session& session, const RequestPdu* request, const ResponsePdu* response) noexcept override;

    void onNAck(Session& session, RequestPdu request, NAckReason reason) noexcept override;

    void readyDestroyed() noexcept override {}

    bool isFinished() noexcept override { return true; }

};


};