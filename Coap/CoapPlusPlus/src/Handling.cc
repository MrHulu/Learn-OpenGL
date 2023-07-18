#include "Handling.h"
#include "coap/DataStruct/BinaryConstView.h"

namespace CoapPlusPlus
{
BinaryConstView Handling::token() const noexcept
{
    return BinaryConstView(*m_token);
}

Handling::Handling(int mid, BinaryConst token) noexcept
    : m_mid(mid), m_token(new BinaryConst(std::move(token)))
{
}

Handling::~Handling() noexcept
{
    if(m_token)
        delete m_token;
}

} // namespace CoapPlusPlus