/**
 * @file SendersManager.cc
 * @author Hulu
 * @brief 客户端发送器管理器
 * @version 0.1
 * @date 2023-07-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once


#include <coap3/coap.h>

namespace CoapPlusPlus
{

class Handling;
class SendersManager
{
public:
    SendersManager(coap_session_t& coap_session);
    ~SendersManager();


};


} // namespace CoapPlusPlus