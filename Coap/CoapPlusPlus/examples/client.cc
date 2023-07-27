/**
 * @file client.cc
 * @author Hulu
 * @brief client example
 * @date 2023-07-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "coap/ContextClient.h"
#include "coap/Log.h"
#include "coap/Session.h"
#include "coap/SendersManager.h"
#include "HandlingExample.h"

#include "coap/Pdu/RequestPdu.h"
#include "coap/Pdu/Options.h"
#include "coap/Pdu/Payload.h"

#include <conio.h>

using namespace CoapPlusPlus;
int main(void)
{
  ContextClient client;

  /* 设置打印等级，大于等于INFO的日志均会打印 */
  Log::SetLevel(LOG_LEVEL::INFO);

  /* 添加客户端会话 */
  if( client.addSession(5688, Information::Udp) == false ) {
    Log::Logging(LOG_LEVEL::ERR, "add session failed!");
    return -1;
  }
  auto session = client.getSession(5688, Information::Udp);
  auto &manager = session->getSendersManager();

  Log::Logging(LOG_LEVEL::INFO, "client start!\n");
  Log::Logging(LOG_LEVEL::INFO, "press G to send Get pdu\n");
  Log::Logging(LOG_LEVEL::INFO, "press P to send Put pdu\n");
  Log::Logging(LOG_LEVEL::INFO, "press D to send Delete pdu\n");
  Log::Logging(LOG_LEVEL::INFO, "press ESC to quit\n");
  while(true) {
    if(_kbhit()) {
      int key = _getch();
      Log::Logging(LOG_LEVEL::INFO, "key: %d\n", key);
      switch(key)
      {
        case 103: // key: g
        case 71:  // key: G
        {
          /* Get消息数据包和响应处理器 */
          auto pdu = manager.createRequest(Information::Confirmable, Information::Get);
          Options options;
          options.insertURIOption("/libcoapcpp/hello");
          pdu.addOptions(options);
          auto handling = std::make_unique<HandlingExample>(std::move(pdu.token().toBinaryConst()));
          if (manager.send(std::move(pdu), std::move(handling)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Get pdu failed!\n");
          }
          break;
        }
        case 112: // key: p
        case 80:  // key: P
        {
          /* Put消息数据包 */
          auto pdu2 = manager.createRequest(Information::NonConfirmable, Information::Put);
          Options options1;
          std::string path1 = "libcoapcpp";
          std::string path2 = "name";
          options1.insert(Information::UriPath, std::vector<uint8_t>(path1.begin(), path1.end()));
          options1.insert(Information::UriPath, std::vector<uint8_t>(path2.begin(), path2.end()));
          options1.insertContentFormatOption(Information::TextPlain);
          pdu2.addOptions(options1);
          Payload payload(4, (uint8_t *)"MOZA", Information::TextPlain);
          pdu2.setPayload(payload);
          auto handling2 = std::make_unique<HandlingExample>(std::move(pdu2.token().toBinaryConst()));
          if (manager.send(std::move(pdu2), std::move(handling2)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Put pdu failed!");
          }
          break;
        }
        case 100: // key: d
        case 68:  // key: D
        {
          /* Delete消息数据包和响应处理器 */
          auto pdu = manager.createRequest(Information::Confirmable, Information::Delete);
          Options options;
          options.insertURIOption("/libcoapcpp/hello");
          pdu.addOptions(options);
          auto handling = std::make_unique<HandlingExample>(std::move(pdu.token().toBinaryConst()));
          if (manager.send(std::move(pdu), std::move(handling)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Delete pdu failed!\n");
          }
          break;
        }
        case 27: // key: ESC
          return 0;
        default: break;
      }
    }
    client.ioProcess(60);
  }

  return 0;
}
