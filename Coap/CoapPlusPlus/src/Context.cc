#include "Context.h"
#include "coap/exception.h"
namespace CoapPlusPlus {


bool Context::startIOProcess(int waitMs) noexcept
{
    if(isReady() == false) {
        coap_log_debug("没有添加endpoint或者session, 无法开始IO处理\n");
        return false;
    }
    stopIOProcess();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_flag = true;
    m_thread = new std::thread(&Context::startIOProcessThreadFunc, this, waitMs);
    return true;
}

void Context::stopIOProcess() noexcept
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_flag = false;
    }
    if(m_thread) {
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }
}

Context::Context() {
    m_ctx = coap_new_context(nullptr);
    if (m_ctx == nullptr) {
        throw CallCoapLibFuncException("Failed to call the coap_new_context function!");
    }
    coap_startup();
    coap_context_set_block_mode(m_ctx, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
    
}

Context::~Context() {
    stopIOProcess();
    if (m_ctx != nullptr) {
        coap_free_context(m_ctx);
        m_ctx = nullptr;
    }
    coap_cleanup();
}

void Context::startIOProcessThreadFunc(int waitMs) noexcept
{
    auto wait_ms = waitMs > 0 ? waitMs : 
    waitMs == 0 ? COAP_IO_WAIT : COAP_IO_NO_WAIT;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_flag = true;
    }
    coap_context_t *temp_ctx;
    while (true) { 
        bool flag; 
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            flag = m_flag;
            temp_ctx = m_ctx;
        }
        if (!flag) break;
        
        auto result = coap_io_process(temp_ctx, wait_ms);
        if (result < 0) 
            break; 
        else if ((uint32_t)result && ((uint32_t)result < wait_ms))
            wait_ms -= result; 
        else {
            std::lock_guard<std::mutex> lock(m_mutex);
            wait_ms = waitMs > 0 ? waitMs : 
            waitMs == 0 ? COAP_IO_WAIT : COAP_IO_NO_WAIT;;
        }
    }
}

};// namespace CoapPlusPlus 