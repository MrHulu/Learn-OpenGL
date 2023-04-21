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

} // namespace Coap