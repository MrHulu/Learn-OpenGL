#include <coap3/coap.h>

namespace CoapPlusPlus {


class EndPoint
{
public:
    /**
     * @brief Construct a new End Point object
     * 
     * @param ctx   上下文
     * @param port  端口号
     * @param proto 网络协议
     * 
     * @exception CallCoapLibFuncException - 创建coap_endpoint_t失败
     */
    EndPoint(coap_endpoint_t* ep);
    ~EndPoint() noexcept;

    const coap_endpoint_t* ep() const noexcept { return m_ep;  }
private:
    coap_endpoint_t* m_ep;
};

};// namespace CoapPlusPlus 