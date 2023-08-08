#include "Context.h"
#include "EventHandling.h"
#include <coap3/coap.h>
#include "coap/exception.h"

namespace CoapPlusPlus {

// bool Context::startIOProcess(int waitMs) noexcept
// {
//     if(isReady() == false) {
//         coap_log_warn("没有添加endpoint或者session, 无法开始IO处理\n");
//         return false;
//     }
//     //stopIOProcess();
//     std::lock_guard<std::mutex> lock(m_mutex);
//     m_flag = true;
//     m_thread = new std::thread(&Context::startIOProcessThreadFunc, this, waitMs);
//     return true;
// }

// void Context::stopIOProcess() noexcept
// {
//     {
//         std::lock_guard<std::mutex> lock(m_mutex);
//         m_flag = false;
//     }
//     if(m_thread) {
//         m_thread->join();
//         delete m_thread;
//         m_thread = nullptr;
//     }
// }

void Context::registerEventHandling(std::unique_ptr<EventHandling> eventHandling) noexcept
{
    if(m_eventHandling != nullptr) {
        delete m_eventHandling;
        m_eventHandling = nullptr;
    }
    if(eventHandling != nullptr) {
        m_eventHandling = eventHandling.release();
        coap_register_event_handler(m_ctx, [](coap_session_t* session, const coap_event_t event) {
            EventHandling::onEvent(static_cast<EventHandling::EventType>(event), session);
            return 0;
        });
    }
    else {
        coap_register_event_handler(m_ctx, nullptr); 
    } 
}

int Context::ioProcess(int waitMs)
{
    if(isReady() == false) {
        throw DataNotReadyException("No endpoint or session added, no network I/O possible.");
    }
    m_isBusy = true;
    auto wait_ms = waitMs > 0 
                    ? waitMs 
                    : waitMs == 0 ? COAP_IO_WAIT : COAP_IO_NO_WAIT;
    auto result = coap_io_process(m_ctx, wait_ms);
    m_isBusy = false;
    return result;
}

bool Context::isioPending() const noexcept
{
    auto result = coap_io_pending(m_ctx);
    return result == 0 ? false : true;
}

Context::Context()
{
    m_ctx = coap_new_context(nullptr);
    if (m_ctx == nullptr) {
        throw InternalException("Failed to call the coap_new_context function!");
    }
    coap_startup();
    coap_context_set_block_mode(m_ctx, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
    coap_set_app_data(m_ctx, this);
}

Context::~Context() {
    while(isBusy());
    if (m_ctx != nullptr) {
        coap_set_app_data(m_ctx, nullptr);
        coap_free_context(m_ctx);
        if(m_eventHandling) {
            delete m_eventHandling;
            m_eventHandling = nullptr;
        }
        m_ctx = nullptr;
    }
    coap_cleanup();
}

// void Context::startIOProcessThreadFunc(int waitMs) noexcept
// {
//     auto wait_ms = waitMs > 0 ? waitMs : 
//     waitMs == 0 ? COAP_IO_WAIT : COAP_IO_NO_WAIT;
//     coap_context_t *temp_ctx;
//     while (true) { 
//         bool flag; 
//         {
//             std::lock_guard<std::mutex> lock(m_mutex);
//             flag = m_flag;
//             temp_ctx = m_ctx;
//         }
//         if (!flag) break;
        
//         auto result = coap_io_process(temp_ctx, wait_ms);
//         if (result < 0) 
//             break; 
//         else if ((uint32_t)result && ((uint32_t)result < wait_ms))
//             wait_ms -= result; 
//         else {
//             wait_ms = waitMs > 0 ? waitMs : 
//             waitMs == 0 ? COAP_IO_WAIT : COAP_IO_NO_WAIT;;
//         }
//     }
// }

};// namespace CoapPlusPlus 