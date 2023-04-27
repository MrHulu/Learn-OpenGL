#include <coap3/coap.h>

#include <map>
#include <thread>
#include <mutex>
#include <memory>

namespace Coap {

class Context;
class EndPoint;
class Resource;

class SimpleServer {
    friend class Resource;
public:
    SimpleServer(Context& ctx);
    ~SimpleServer();

    /**
     * @brief 启动服务器
     * 
     * @param timeout_ms 表示等待处理I/O事件的最长时间（以毫秒为单位）。默认为-1，表示立即返回。
     *      @c -1: 立即返回
     *      @c 0: 一直等待
     *      @c 其他: 等待指定的时间
     * 
     * @exception std::runtime_error 启动服务器失败, 服务器还没添加网络协议端口
     */
    void start(int timeout_ms = -1);

    /**
     * @brief 停止服务器
     * 
     */
    void stop();

    void addTextResource(const std::string& url);

    /**
     * @brief 为服务器添加一个资源
     * 
     * @param resource 
     * 
     * @exception std::invalid_argument 参数为空或者已经存在相同的资源
     */
    void addResource(std::shared_ptr<Coap::Resource> resource);

private:
    /* 管理 */
    Context& ctx;
    std::map<std::string, std::shared_ptr<Resource>> m_resources;
    std::thread* m_thread {};

    /* 其他 */
    std::mutex m_mutex;
    bool m_flag = true;
    int  m_timeout_ms = COAP_IO_WAIT;

private:
    void startCoapProcess();
};


}; // namespace Coap