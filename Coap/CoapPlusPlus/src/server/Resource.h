#pragma once

#include <coap3/coap.h>
#include <string>

namespace CoapPlusPlus {

class SimpleServer;
class Resource 
{
    friend class SimpleServer;
public:
    virtual ~Resource();

    /**
     * @brief Get the Uri Path object
     * 
     * @return 返回资源的url路径
     */
    std::string getUriPath() const noexcept { return m_uriPath; }

protected:
    /**
     * @brief Construct a new Resource object
     * 
     * @param uriPath 资源路径
     * @param observable 是否可观察，默认为false
     * 
     * @exception CallCoapLibFuncException 资源创建失败
     */
    Resource(const std::string& uriPath, bool observable = false);
    void notifyObservers() noexcept;

    /**
     * @brief Get the Resource object
     * 
     * @return coap_resource_t* 
     */
    coap_resource_t* getResource() const noexcept { return m_resource; }

    /**
     * @brief 删除资源
     * 
     * @note 该函数会释放coap资源
     */
    void freeResource() noexcept;

    virtual void getRequestCallback(coap_session_t* session,
                                const coap_pdu_t* request, const coap_string_t* query, 
                                coap_pdu_t* response) = 0;
    virtual void postRequestCallback(coap_session_t* session,
                                const coap_pdu_t* request, const coap_string_t* query, 
                                coap_pdu_t* response) = 0;
    virtual void putRequestCallback(coap_session_t* session,
                                const coap_pdu_t* request, const coap_string_t* query, 
                                coap_pdu_t* response) = 0;
    virtual void deleteRequestCallback(coap_session_t* session,
                                const coap_pdu_t* request, const coap_string_t* query, 
                                coap_pdu_t* response) = 0;


private:
    static void getRequestCallbackWrapper(coap_resource_t* resource, coap_session_t* session,
                                        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void postRequestCallbackWrapper(coap_resource_t* resource, coap_session_t* session,
                                        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void putRequestCallbackWrapper(coap_resource_t* resource, coap_session_t* session,
                                        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    static void deleteRequestCallbackWrapper(coap_resource_t* resource, coap_session_t* session,
                                        const coap_pdu_t* request, const coap_string_t* query, coap_pdu_t* response);
    /**
     * @brief 内部私有函数，重置资源
     * 
     * @note 该函数会在Server析构时被调用
     */
    void resetResource() noexcept { m_resource = nullptr; }

    /**
     * @brief 内部私有函数，设置该资源的上级服务器
     * 
     * @param server
     * 
     * @note 该函数会在注册到服务器时被调用
     */
    void setServer(SimpleServer* server) noexcept { this->server = server; }

protected:
    coap_resource_t* m_resource;
    std::string m_uriPath;

private:
    SimpleServer* server;

};

};// namespace CoapPlusPlus 