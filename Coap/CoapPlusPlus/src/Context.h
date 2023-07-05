/**
 * @file Context.h
 * @author Hulu
 * @brief Context基类定义
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <coap3/coap.h>
#include <thread>
#include <mutex>
#include <memory>

namespace CoapPlusPlus {

class Context 
{
public:
    /**
     * @brief 开始悬而未决的网络I/O，然后选择性地等待下一个输入数据包。
     * 
     * 
     * @param waitMs 在做完任何处理后返回前等待新数据包的最小毫秒数。
     *               如果为0，该调用将阻塞到下一个内部动作（如数据包重传）（如果有），或阻塞到收到下一个数据包（以较早者为准），并做必要的处理。
     *               如果为-1，该函数将在处理后立即返回，而不等待任何新的输入数据包到达。
     * @return 开始IO处理，成功返回true，否则返回false
     */
    bool startIOProcess(int waitMs = 1000) noexcept;

    /**
     * @brief 停止阻塞当前的Coap I/O 处理
     * 
     */
    void stopIOProcess() noexcept;

protected:
    /**
     * @brief coap_context_t的C++封装，一个Context对应一个服务器或者一个客户端。
     *        CoAP协议栈的全局状态被存储在coap_context_t对象中。
     * 
     * @exception CallCoapLibFuncException 创建Context失败
     */
    Context();
    ~Context() noexcept;
    coap_context_t* getContext() const noexcept { return m_ctx; }

    /**
     * @brief 是否准备好一切以用于网络I/O
     * 
     * @return true 
     * @return false 
     */
    virtual bool isReady() const noexcept = 0;

protected :
    coap_context_t*     m_ctx {};

private:
    std::thread* m_thread {};
    std::mutex m_mutex;
    bool m_flag = true;

private: 
    void startIOProcessThreadFunc(int waitMs) noexcept;
};


};// namespace CoapPlusPlus 