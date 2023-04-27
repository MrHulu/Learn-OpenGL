#pragma once

#include <coap3/coap.h>
#include <map>
#include <memory>
#include <map>

namespace Coap {

class SimpleClient;
class SimpleServer;
class EndPoint;

class Context 
{
    friend class SimpleClient;
    friend class SimpleServer;
public:
    /**
     * @brief coap_context_t的C++封装，一个Context对应一个服务器或者一个客户端。
     *      CoAP协议栈的全局状态被存储在coap_context_t对象中。
     * 
     * @exception CallCoapLibFuncException 创建Context失败
     */
    Context();
    ~Context() noexcept;

    /**
     * @brief 获取与Context相联系的服务器对象
     * 
     * @return std::shared_ptr<SimpleServer> 
     */
    std::shared_ptr<SimpleServer> server() noexcept;
    
    /**
     * @brief 获取与Context相联系的客户端对象
     * 
     * @return std::shared_ptr<SimpleClient> 
     */
    std::shared_ptr<SimpleClient> client() noexcept;

    /**
     * @brief 为Context添加一个Coap协议的网络，用于与客户端通信。
     *      
     * @param port 端口号，默认为5683
     * @param proto 传输层协议,默认为UDP协议
     *      @c 1: UDP协议
     *      @c 2: TCP协议
     *      @c 3: TLS协议
     *      @c 4: DTLS协议
     * 
     * @return std::shared_ptr<EndPoint>
     * 
     * @exception std::invalid_argument 已经存在相同地址的传输层协议
     * @exception CallCoapLibFuncException 创建EndPoint失败
     */
    void addEndPoint(int port = 5683, int proto = 1);

    /**
     * @brief 为Context移除一个Coap协议的网络
     * 
     * @param port 端口号，默认为5683
     * 
     * @exception std::invalid_argument 无法找到相应的EndPoint
     */
    void removeEndPoint(int port = 5683);

    /**
     * @brief 获取与Context相联系的网络协议数量
     * 
     * @return int
     */
    int endPointCount() const noexcept { return m_endpoints.size(); }

private:
    coap_endpoint_t* createEndPoint(int port, int proto);

private:
    std::shared_ptr<SimpleClient> m_client;
    std::shared_ptr<SimpleServer> m_server;
    std::map<int, std::shared_ptr<EndPoint>> m_endpoints;

private :
    coap_context_t*     m_ctx {};
};


}; // namespace Coap