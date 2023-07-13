#pragma once

#include <stdexcept>
#include <string>

namespace CoapPlusPlus {

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
 * @brief 内部错误
 * 
 */
class InternalException : public std::runtime_error
{
public:
    explicit InternalException(const std::string& what) : std::runtime_error(what) { }
    explicit InternalException(const char* what) : std::runtime_error(what) { }    
};

/**
 * @brief 当数据被销毁后继续访问时抛出此异常
 * 
 */
class DataWasReleasedException : public std::runtime_error
{
public:
    explicit DataWasReleasedException(const std::string& what) : std::runtime_error(what) { }
    explicit DataWasReleasedException(const char* what) : std::runtime_error(what) { }    
};

/**
 * @brief 要操作或者访问的目标不存在时抛出此异常
 * 
 */
class TargetNotFoundException : public std::runtime_error
{
public:
    explicit TargetNotFoundException(const std::string& what) : std::runtime_error(what) { }
    explicit TargetNotFoundException(const char* what) : std::runtime_error(what) { }
};


}// namespace CoapPlusPlus