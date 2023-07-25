#include "SendersManager.h"
#include <coap3/coap.h>

namespace CoapPlusPlus
{

class SendersManager::SendersManagerHandlerWrapper {

public:    
    static void NackHandler(coap_session_t *session, const coap_pdu_t *sent, coap_nack_reason_t reason, const coap_mid_t mid);
    static coap_response_t AckHandler(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, const coap_mid_t mid);
};



} // namespace CoapPlusPlus
