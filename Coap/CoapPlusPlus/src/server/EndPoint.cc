#include "EndPoint.h"
#include "coap/exception.h"

namespace CoapPlusPlus {

EndPoint::EndPoint(coap_endpoint_t *ep) : m_ep(ep)
{
    
}

EndPoint::~EndPoint() noexcept
{
    // 已被coap_free_context释放
    //coap_free_endpoint(m_ep);
}

};// namespace CoapPlusPlus 