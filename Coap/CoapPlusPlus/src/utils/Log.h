/**
 * @file Log.h
 * @author Hulu
 * @brief coap中的日志系统
 * @version 0.2
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <functional>
#include <mutex>
#include <string>

namespace CoapPlusPlus
{

enum LOG_LEVEL
{
    EMERG = 0,
    ALERT = 1,
    CRIT,
    ERR,
    WARN,
    NOTICE,
    INFO,
    DEBUG,
    OSCORE,
    DTLS_BASE,
};

/**
 * @brief 日志系统
 * 
 * @note 该类是线程安全的，不过在多线程环境下，日志的输出顺序可能会不一致
 */
class Log 
{
    Log() {}
    ~Log() {}
    static std::mutex logMutex;
    static std::function<void(LOG_LEVEL, const char*)> logHandler;
public:

    /**
     * @brief Set the Log Handler object
     * 
     * @param function 
     */
    static void SetLogHandler(std::function<void(LOG_LEVEL, const char*)> function) noexcept;

    /**
     * @brief 记录日志
     * 
     * @param level 日志等级 
     * @param fmt 格式化字符串
     */
    static void Logging(LOG_LEVEL level, const char* fmt, ...) noexcept;

    /**
     * @brief 获取当前版本号
     * 
     * @return std::string 如 1.0.0
     */
    static std::string GetCurrentVersion() noexcept;

    /**
     * @brief 获取当前日志等级
     * 
     * @return LOG_LEVEL 
     */
    static LOG_LEVEL GetLevel() noexcept;

    /**
     * @brief 设置日志等级，低于该等级的日志将不会被打印
     * 
     * @param level 日志等级, LOG_LEVEL::EMERG 为最高等级
     */
    static void SetLevel(LOG_LEVEL level) noexcept;

    /**
     * @brief 将日志等级转换为字符串
     * 
     * @param level 日志等级
     * @return std::string 
     */
    static std::string LevelToString(LOG_LEVEL level) noexcept;

private:
    /* data */

};



} // namespace CoapPlusPlus