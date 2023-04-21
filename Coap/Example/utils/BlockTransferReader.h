#pragma once

#include <coap3/coap.h>
#include "CoapException.h"

namespace Coap {

class BlockTransferReader
{
public:
    /**
     * @brief Construct a new Block Transfer Reader object
     * 
     * @param session 
     * @param request 
     * 
     * @exception std::invalid_argument if session or request is nullptr.
     * @exception std::runtime_error if called libcoap function failed.
     */
    BlockTransferReader(coap_session_t *session, const coap_pdu_t *request);
    ~BlockTransferReader() noexcept;

    /**
     * @brief Check if the request is a block transfer request.
     * 
     * @return true 
     * @return false 
     */
    bool isBlockTransfer() const noexcept;

    /**
     * @brief Check if the block transfer is complete.
     * 
     * @return true 
     * @return false
     */
    bool isBlockTransferComplete() const noexcept;

    /**
     * @brief Get the data from the block transfer.
     * 
     * @return coap_binary_t* 
     * 
     * @note The caller is responsible for freeing the memory.
     * 
     * @exception DataNotReadyException if the block transfer is not complete.
     * @exception std::runtime_error if called libcoap function failed.
     */
    coap_binary_t* read();

private:
    coap_session_t* m_session;
    const coap_pdu_t* m_request;

    size_t m_blockSize = 0;
    size_t m_blockOffset = 0;
    size_t m_blockTotal = 0;
    coap_binary_t* m_bobyData = nullptr;
    const uint8_t* m_data = nullptr;
    bool m_isBlockTransfer = false;

private:
    coap_cache_entry_t *getCacheEntry() const noexcept;
    void handleBlockTransfer() const;
};


}; // namespace Coap