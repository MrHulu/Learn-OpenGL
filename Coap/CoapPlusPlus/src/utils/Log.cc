#include <coap3/coap.h>
#include "Log.h"

namespace CoapPlusPlus
{

std::mutex Log::logMutex;
std::function<void(LOG_LEVEL, const char*)> Log::logHandler;

void Log::SetLogHandler(std::function<void(LOG_LEVEL, const char*)> function) noexcept
{
    std::lock_guard<std::mutex> lock(logMutex);
    logHandler = function;
    coap_set_log_handler([](coap_log_t level, const char* message){
        if(logHandler)
            logHandler(static_cast<LOG_LEVEL>(level), message);
    });
}

void Log::Logging(LOG_LEVEL level, const char* fmt, ...) noexcept
{
    va_list args;
    char buffer[1024];
    va_start(args, fmt);
    auto count = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    if(count >= 1024)
        return ;
    {
        std::lock_guard<std::mutex> lock(logMutex);
        coap_log(static_cast<coap_log_t>(level), buffer);
    }
}

LOG_LEVEL Log::GetLevel() noexcept
{
    std::lock_guard<std::mutex> lock(logMutex);
    return static_cast<LOG_LEVEL>(coap_get_log_level());
}

void Log::SetLevel(LOG_LEVEL level) noexcept
{
    std::lock_guard<std::mutex> lock(logMutex);
    coap_set_log_level(static_cast<coap_log_t>(level));
}

std::string Log::LevelToString(LOG_LEVEL level) noexcept
{
    switch (level)
    {
    case LOG_LEVEL::EMERG:
        return "EMERG";
    case LOG_LEVEL::ALERT:
        return "ALERT";
    case LOG_LEVEL::CRIT:
        return "CRIT";
    case LOG_LEVEL::ERR:
        return "ERR";
    default:
    case LOG_LEVEL::WARN:
        return "WARN";
    case LOG_LEVEL::NOTICE:
        return "NOTICE";
    case LOG_LEVEL::INFO:
        return "INFO";
    case LOG_LEVEL::DEBUG:
        return "DEBUG";
    case LOG_LEVEL::OSCORE:
        return "OSCORE";
    case LOG_LEVEL::DTLS_BASE:
        return "DTLS_BASE";
    }
}

} // namespace CoapPlusPlus