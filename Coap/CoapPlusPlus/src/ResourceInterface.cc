#include "ResourceInterface.h"
#include "Resource.h"

namespace CoapPlusPlus {


void ResourceInterface::setResource(Resource *resource)
{
    r = resource;
}

void ResourceInterface::notifyObserver() noexcept
{
    if(r)
        r->notifyObserver();
}


};