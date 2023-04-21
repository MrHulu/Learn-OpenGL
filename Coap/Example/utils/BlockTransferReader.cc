#include "BlockTransferReader.h"

#include <string>
#include <stdexcept>

Coap::BlockTransferReader::BlockTransferReader(coap_session_t *session, const coap_pdu_t *request)
    : m_session(session), m_request(request)
{
    if(m_session == nullptr && m_request == nullptr) {
        throw std::invalid_argument("Invalid session or request.");
    }
    auto temp = coap_get_data_large(m_request, &m_blockSize, &m_data, &m_blockOffset, &m_blockTotal);
    m_isBlockTransfer = temp && m_blockSize != m_blockTotal;
    handleBlockTransfer();
}

Coap::BlockTransferReader::~BlockTransferReader() noexcept
{

}

bool Coap::BlockTransferReader::isBlockTransfer() const noexcept
{
    return m_isBlockTransfer;
}

bool Coap::BlockTransferReader::isBlockTransferComplete() const noexcept
{
    return m_blockOffset + m_blockSize == m_blockTotal;
}

coap_binary_t *Coap::BlockTransferReader::read()
{
    if(m_bobyData == nullptr) {
        if(isBlockTransfer() == true) {
            if(isBlockTransferComplete() == true) {            
                auto cache_entry = getCacheEntry();
                m_bobyData = static_cast<coap_binary_t *>(coap_cache_get_app_data(cache_entry));
                coap_cache_set_app_data(cache_entry, nullptr, nullptr);
            } else {
                throw DataNotReadyException("Block transfer is not complete.");
                // coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTINUE);
            }
        } else {
            m_bobyData = coap_new_binary(m_blockSize);
            if (m_bobyData) {
                memcpy(m_bobyData->s, m_data, m_blockSize);
            } else {
                throw std::runtime_error("Unable to allocate memory for the body data.");
            }
        } 
    }
    return m_bobyData;
}

coap_cache_entry_t *Coap::BlockTransferReader::getCacheEntry() const noexcept
{
    // 尝试从缓存中获取与当前请求和会话相关的缓存条目
    auto cache_entry = coap_cache_get_by_pdu(m_session, m_request, COAP_CACHE_IS_SESSION_BASED);

    // 如果块偏移为0，则表示这是一个新的块传输请求
    if (m_blockOffset == 0) {
        
        // 如果缓存条目不存在就创建新的缓存条目
        if (!cache_entry) {
            cache_entry = coap_new_cache_entry(m_session, m_request, COAP_CACHE_NOT_RECORD_PDU, COAP_CACHE_IS_SESSION_BASED, 0);
        } 
        // 如果应用数据存在, 则删除与之关联的应用数据，并将应用数据设置为空。
        else {
            auto data = static_cast<coap_binary_t *>(coap_cache_get_app_data(cache_entry));
            if (data) {
                coap_delete_binary(data);
                data =  nullptr;
            }
            coap_cache_set_app_data(cache_entry, nullptr, nullptr);
        }
    }

    return cache_entry;
}

void Coap::BlockTransferReader::handleBlockTransfer() const
{
    auto cache_entry = getCacheEntry();
    
    // 异常处理
    if (!cache_entry) {
        if (m_blockOffset == 0) {
            std::string msg = "Unable to create a new cache entry\n";
            coap_log(LOG_WARNING, msg.c_str());
            throw std::runtime_error(msg);
        } else {
            std::string msg = "No cache entry available for the non-first BLOCK\n";
            coap_log(LOG_WARNING, msg.c_str());
            throw std::runtime_error(msg);
        }
        // coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    }

    // 如果块数据的数量有效，从缓存条目中获取应用数据，并构建数据块。然后设置应用数据。
    if (m_blockSize != 0) {
        auto data = static_cast<coap_binary_t *>(coap_cache_get_app_data(cache_entry));
        data = coap_block_build_body(data, m_blockSize, m_data, m_blockOffset, m_blockTotal);
        coap_cache_set_app_data(cache_entry, data, NULL);
    }

}
