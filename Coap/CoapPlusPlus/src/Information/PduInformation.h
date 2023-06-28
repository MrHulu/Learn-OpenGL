#pragma once


namespace CoapPlusPlus {
namespace Information {

enum MessageType {
    Confirmable = 0,
    NonConfirmable = 1,
    Acknowledgement = 2,
    Reset = 3
};

enum ResponseCode {
    Created = 65,   // 2.01 已创建
    Deleted = 66,   // 2.02 已删除
    Valid = 67,     // 2.03 有效
    Changed = 68,   // 2.04 已更改
    Content = 69,   // 2.05 内容
    Continue = 95,  // 2.31 继续
    BadRequest = 128,   // 4.00 错误请求
    Unauthorized = 129, // 4.01 未授权
    BadOption = 130,    // 4.02 错误的选项
    Forbidden = 131,    // 4.03 禁止
    NotFound = 132,     // 4.04 未找到
    MethodNotAllowed = 133, // 4.05 方法不允许
    NotAcceptable = 134,    // 4.06 不可接受
    Incomplete = 136,       // 4.08 请求实体不完整
    Conflict = 137,         // 4.09 冲突
    PreconditionFailed = 140,       // 4.12 前提条件失败
    RequestEntityTooLarge = 141,    // 4.13 请求实体太大
    UnsupportedContentFormat = 143, // 4.15 不支持的内容格式
    Unprocessable = 150,            // 4.22 无法处理的实体
    TooManyRequests = 157,          // 4.29 请求过多
    InternalServerError = 160,      // 5.00 内部服务器错误
    NotImplemented = 161,           // 5.01 未实现
    BadGateway = 162,               // 5.02 错误的网关
    ServiceUnavailable = 163,       // 5.03 服务不可用
    GatewayTimeout = 164,           // 5.04 网关超时
    ProxyingNotSupported = 165,     // 5.05 不支持代理
    HopLimitReached = 168           // 5.08 跳限制已达到
};


enum RequestCode {
    Get = 1,
    Post = 2,
    Put = 3,
    Delete = 4,
    Fetch = 5,
    Patch = 6,
    IPatch = 7
};

class PduInformation {
    PduInformation() = delete;
    ~PduInformation() = delete;
public:
    /**
     * @brief 客户端从响应码中得到一个Coap状态头
     * @param code 响应码
     * @return 响应的状态头
     *     @retval 0 表示空报文，收到一个ACK或者RST报文，或者收到一个请求报文，但是没有找到对应的资源。
     *     @retval 2 表示正确响应，请求已成功被服务器接收、理解、并接受。
     *     @retval 4 表示客户端错误，客户端看起来可能发生了错误，妨碍了服务器的处理。
     *     @retval 5 表示服务端错误，在处理请求的过程中有错误或者异常状态发生或者当前的软硬件资源无法完成对请求的处理。
    */
    static int getResponseStatusHead(ResponseCode code)  { return (((code) >> 5) & 0xFF); }

    /**
     * @brief 客户端从响应的code得到一个Coap状态码
     * 
     * @param code 响应的code 
     * @return Coap状态
     *      @retval 201 ResponseCode::Created（资源已成功创建）
     *      @retval 202 ResponseCode::Deleted（资源已成功删除）
     *      @retval 203 ResponseCode::Valid（请求已验证，但不会返回新数据）
     *      @retval 204 ResponseCode::Changed（资源已成功更新）
     *      @retval 205 ResponseCode::Content（请求成功，响应消息包含所请求的内容）
     *      @retval 400 ResponseCode::BadRequest（请求无效或不完整）
     *      @retval 401 ResponseCode::Unauthorized（未授权，请求需要认证）
     *      @retval 402 ResponseCode::Bad ption（请求包含无法识别的选项）
     *      @retval 403 ResponseCode::Forbidden（请求目标无权访问）
     *      @retval 404 ResponseCode::NotFound（请求的资源不存在）
     *      @retval 405 ResponseCode::MethodNotAllowed（请求方法不允许使用）
     *      @retval 406 ResponseCode::NotAcceptable（请求的内容格式不受服务器支持）
     *      @retval 408 ResponseCode::Incomplete（请求的实体不完整）
     *      @retval 409 ResponseCode::Conflict（请求的资源存在冲突）
     *      @retval 412 ResponseCode::PreconditionFailed（前提条件失败
     *      @retval 413 ResponseCode::RequestEntityTooLarge（请求的实体大小超出服务器处理能力）
     *      @retval 415 ResponseCode::UnsupportedContentFormat（请求的内容格式不受服务器支持）
     *      @retval 422 ResponseCode::Unprocessable（请求格式正确，但是由于含有语义错误，无法响应）
     *      @retval 429 ResponseCode::TooManyRequests（请求过多）
     *      @retval 500 ResponseCode::InternalServerError（内部服务器错误）
     *      @retval 501 ResponseCode::NotImplemented（服务器不支持请求的功能）
     *      @retval 502 ResponseCode::BadGateway（服务器作为网关或代理，从上游服务器收到无效响应）
     *      @retval 503 ResponseCode::ServiceUnavailable（服务不可用）
     *      @retval 504 ResponseCode::GatewayTimeout（服务器作为网关或代理没有及时从上游收到响应）
     *      @retval 505 ResponseCode::ProxyingNotSupported（代理不支持）
     *      @retval 508 ResponseCode::HopLimitReached（消息在转发过程中达到了跳限制）
     */
    static int getResponseStatusCode(ResponseCode code) { 
        auto head = getResponseStatusHead(code);
        return head * 100 + (code & 0x1F);
    }
};

};// namespace Information
};// namespace CoapPlusPlus
