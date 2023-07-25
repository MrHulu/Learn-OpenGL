#include <coap3/coap.h>
#include "ResourceManager.h"
#include "Resource.h"
#include "coap/exception.h"

namespace CoapPlusPlus
{

ResourceManager::ResourceManager(ContextServer& context)
    : _context(context)
{
}

ResourceManager::~ResourceManager() noexcept
{
    for (auto& pair : m_resources) {
        delete pair.second; // TODO: 由Context删除？看了源码发现 context = resource->context; RESOURCES_DELETE(context->resources, resource);
    }
    m_resources.clear();
}

bool ResourceManager::registerResource(std::unique_ptr<Resource> resource) noexcept
{
    if (resource) {
        auto uriPath = resource->getUriPath();
        if (m_resources.find(uriPath) == m_resources.end()) {
            resource->initResource();
            coap_add_resource(_context.getContext(), resource->getResource());
            m_resources[uriPath] = resource.release();
            return true;
        }
    }
    return false;
}

bool ResourceManager::unregisterResource(const std::string& uriPath) noexcept
{
    auto it = m_resources.find(uriPath);
    if (it != m_resources.end())
    {
        delete it->second;
        m_resources.erase(it);
        return true;
    }
    else
        return false;
}

} // namespace CoapPlusPlus
