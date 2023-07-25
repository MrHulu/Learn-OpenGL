#include <coap3/coap.h>
#include "EndPoint.h"

namespace CoapPlusPlus
{

EndPoint::~EndPoint() noexcept
{ 
    coap_free_endpoint(m_ep); 
}

}; // namespace CoapPlusPlus