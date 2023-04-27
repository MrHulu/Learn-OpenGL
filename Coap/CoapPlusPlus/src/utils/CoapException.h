#pragma once

#include <stdexcept>
#include <string>

namespace Coap {

/**
 * @brief Exception thrown when data is not ready to be read.
 * 
 */
class DataNotReadyException : public std::runtime_error
{
public:
    explicit DataNotReadyException(const std::string& what) : std::runtime_error(what) { }
    explicit DataNotReadyException(const char* what) : std::runtime_error(what) { }
};

/**
 * @brief 当调用libcoap库失败时抛出此异常
 * 
 */
class CallCoapLibFuncException : public std::runtime_error
{
public:
    explicit CallCoapLibFuncException(const std::string& what) : std::runtime_error(what) { }
    explicit CallCoapLibFuncException(const char* what) : std::runtime_error(what) { }    
};

} // namespace Coap