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

#include <thread>
#include <mutex>
#include <memory>

struct coap_context_t;
namespace CoapPlusPlus {

class EventHandling;
class Context 
{
    Context& operator=(const Context&) = delete;
    Context& operator=(Context&&) = delete;
    Context(const Context&) = delete;
    Context(Context&&) = delete;
public:
    // /**
    //  * @brief 开始悬而未决的网络I/O，然后选择性地等待下一个输入数据包。
    //  * 
    //  * 
    //  * @param waitMs 在做完任何处理后返回前等待新数据包的最小毫秒数。
    //  *               如果为0，该调用将阻塞到下一个内部动作（如数据包重传）（如果有），或阻塞到收到下一个数据包（以较早者为准），并做必要的处理。
    //  *               如果为-1，该函数将在处理后立即返回，而不等待任何新的输入数据包到达。
    //  * @return 开始IO处理，成功返回true，否则返回false
    //  */
    // bool startIOProcess(int waitMs = 1000) noexcept;

    // /**
    //  * @brief 停止阻塞当前的Coap I/O 处理
    //  * 
    //  */
    // void stopIOProcess() noexcept;

    /**
     * @brief 注册一个事件处理器，用于处理Coap事件, 如果要取消注册，可以将参数设置为nullptr
     * 
     * @param eventHandling 事件处理器，生命周期将由class Context管理 @see EventHandling
     */
    void registerEventHandling(std::unique_ptr<EventHandling> eventHandling) noexcept;

    /**
     * @brief 进行一次网络I/O的处理
     * 
     * @param waitMs 在做完任何处理后返回前等待新数据包的最小毫秒数。
     *               如果为0，该调用将阻塞到下一个内部动作（如数据包重传）（如果有），或阻塞到收到下一个数据包（以较早者为准），并做必要的处理。
     *               如果为-1，该函数将在处理后立即返回，而不等待任何新的输入数据包到达。
     * @return 返回在函数中花费的毫秒数；如果出现错误，则返回 -1。 
     * 
     * @exception DataNotReadyException 数据未准备好，无法进行网络I
     */
    int ioProcess(int waitMs = 1000);

    /**
     * @brief 检查是否有任何 I/O 待处理。
     * 
     * @return true io有待处理
     * @return false 没有io待处理
     */
    bool isioPending() const noexcept;

    /**
     * @brief 是否正在进行网络I/O
     * 
     * @return true 正在进行网络I/O
     * @return false 没有进行网络I/O
     */
    bool isBusy() const noexcept { return m_isBusy; }

protected:
    /**
     * @brief coap_context_t的C++封装，一个Context对应一个服务器或者一个客户端。
     *        CoAP协议栈的全局状态被存储在coap_context_t对象中。
     * 
     * @exception InternalException 创建Context失败
     */
    Context();
    virtual ~Context() noexcept;
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
    //bool m_flag = true;
    bool m_isBusy = false;

private: 
    EventHandling* m_eventHandling = nullptr;
};


};// namespace CoapPlusPlus 