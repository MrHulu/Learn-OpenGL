/* minimal CoAP server
 *
 * Copyright (C) 2018-2023 Olaf Bergmann <bergmann@tzi.org>
 */

#include "coap/ContextServer.h"
#include "coap/Resource.h"
#include "coap/ResourceManager.h"
#include "ResourceInterfaceExample.h"
#include <string>

using namespace CoapPlusPlus;
int main(void)
{
  ContextServer server;

  /* 设置程序的打印等级，大于等于INFO的日志均会打印 */
  Log::SetLevel(LOG_LEVEL::INFO);

  /* 添加网络端点 */
  if (server.addEndPoint(5688, Information::Udp) == false)
  {
    Log::Logging(LOG_LEVEL::ERR, "add endpoint failed!");
    return -1;
  }
  auto& manager = server.getResourceManager();

  /* 注册hello资源 */
  auto helloResource = std::make_unique<Resource>("libcoapcpp/hello", false);
  helloResource->registerInterface(std::make_unique<ResourceInterfaceExample>("world", Information::RequestCode::Get));
  if (manager.registerResource(std::move(helloResource)) == false)  // 资源对象将由资源管理器接管，不需要用户手动释放
  {
    Log::Logging(LOG_LEVEL::ERR, "register hello resource failed!");
    return -1;
  }

  /* 注册name资源 */
  auto nameResource = std::make_unique<Resource>("libcoapcpp/name", false);
  nameResource->registerInterface(std::make_unique<ResourceInterfaceExample>("Gudsen", Information::RequestCode::Put));
  if (manager.registerResource(std::move(nameResource)) == false)  // 资源对象将由资源管理器接管，不需要用户手动释放
  {
    Log::Logging(LOG_LEVEL::ERR, "register name resource failed!");
    return -1;
  }

  Log::Logging(LOG_LEVEL::INFO, "server start!\n");
  //std::flush(std::cout);
  while (true)
  {
    server.ioProcess(0);
  }

  return 0;
}
