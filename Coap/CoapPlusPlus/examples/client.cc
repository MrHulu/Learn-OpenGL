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

#ifdef _WIN32  // Windows
  #include <conio.h>
#else  // Linux
  #include <iostream>
  #include <sys/select.h>
  #include <termios.h>
  #include <unistd.h>

  int _kbhit()
  {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
  }

  int _getch()
  {
    int ch;
    struct termios old_termios, new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    return ch;
  }
#endif

using namespace CoapPlusPlus;
int main(void)
{
  ContextClient client;

  /* 设置打印等级，大于等于INFO的日志均会打印 */
  Log::SetLevel(LOG_LEVEL::INFO);

  /* 添加客户端会话 */
  if( client.addSession(40266, Information::Udp) == false ) {
    Log::Logging(LOG_LEVEL::ERR, "add session failed!");
    return -1;
  }
  auto session = client.getSession(40266, Information::Udp);
  auto &manager = session->getSendersManager();

  auto state = false;
  client.registerHandshakeResponedFunction([&state](const CoapPlusPlus::Session* session, 
                                                const CoapPlusPlus::ResponsePdu* response, int id) 
    {
        if(session) {
            state = true;
            Log::Logging(LOG_LEVEL::INFO, "handshake success!\n");
        }
    });
  client.setHandshakeInterval(3);

  Log::Logging(LOG_LEVEL::INFO, "client start!\n");
  Log::Logging(LOG_LEVEL::INFO, "press G to send Get pdu\n");
  Log::Logging(LOG_LEVEL::INFO, "press P to send Put pdu\n");
  Log::Logging(LOG_LEVEL::INFO, "press D to send Delete pdu\n");
  Log::Logging(LOG_LEVEL::INFO, "press ESC to quit\n");
  auto getflag = true;
  auto putflag = true;
  
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
          options.insertURIOption("/MOZARacing/ProductDevice");
          options.insertOsberveOption(true);
          pdu.addOptions(options);
          auto handling = std::make_unique<HandlingExample>(std::move(pdu.token().toBinaryConst()));
          if (manager.send(std::move(pdu), std::move(handling)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Get pdu failed!\n");
          }
          break;
        }
        case 49:  // key 1
        {
          auto pdu = manager.createRequest(Information::Confirmable, Information::Get);
          Options options;
          options.insertURIOption("/MOZARacing/ProductDevice/050d36ef6f3930e4");
          pdu.addOptions(options);
          auto handling = std::make_unique<HandlingExample>(std::move(pdu.token().toBinaryConst()));
          manager.send(std::move(pdu), std::move(handling));
          break;
        }
        case 50: // key 2
        {
          auto pdu = manager.createRequest(Information::Confirmable, Information::Get);
          Options options;
          options.insertOsberveOption(true);
          options.insertURIOption("/MOZARacing/ProductDevice/5853f8eb/AccOutDir");
          pdu.addOptions(options);
          auto handling = std::make_unique<HandlingExample>(std::move(pdu.token().toBinaryConst()));
          manager.send(std::move(pdu), std::move(handling));
          break;
        }
        case 51: // key 3
        {
          auto pdu = manager.createRequest(Information::Confirmable, Information::Get);
          Options options;
          options.insertOsberveOption(true);
          options.insertURIOption("/MOZARacing/ProductDevice/050d36ef6f3930e4/NaturalFriction");
          pdu.addOptions(options);
          auto handling = std::make_unique<HandlingExample>(std::move(pdu.token().toBinaryConst()));
          manager.send(std::move(pdu), std::move(handling));
          break;
        }
        case 52: //key 4
        {
          auto pdu = manager.createRequest(Information::Confirmable, Information::Get);
          Options options;
          options.insertOsberveOption(true);
          options.insertURIOption("/MOZARacing/ProductDevice/050d36ef6f3930e4/LimitAngle");
          pdu.addOptions(options);
          auto handling = std::make_unique<HandlingExample>(std::move(pdu.token().toBinaryConst()));
          manager.send(std::move(pdu), std::move(handling));
          break;
        }
        case 53: // key: 5
        {
          auto pdu = manager.createRequest(Information::Confirmable, Information::Get);
          Options options;
          options.insertOsberveOption(true);
          options.insertURIOption("/MOZARacing/ProductDevice/050d36ef6f3930e4/ScreenUIList");
          pdu.addOptions(options);
          auto handling = std::make_unique<HandlingExample>(std::move(pdu.token().toBinaryConst()));
          manager.send(std::move(pdu), std::move(handling));
          break;
        }
        case 112: // key: p 
        {
            /* Put消息数据包 */
          auto pdu2 = manager.createRequest(Information::Confirmable, Information::Put);
          Options options1;
          std::string path1 = "MOZARacing";
          std::string path2 = "ProductDevice";
          std::string path3 = "5853f8eb";
          std::string path4 = "AccOutDir";
          options1.insert(Information::UriPath, std::vector<uint8_t>(path1.begin(), path1.end()));
          options1.insert(Information::UriPath, std::vector<uint8_t>(path2.begin(), path2.end()));
          options1.insert(Information::UriPath, std::vector<uint8_t>(path3.begin(), path3.end()));
          options1.insert(Information::UriPath, std::vector<uint8_t>(path4.begin(), path4.end()));
          options1.insertContentFormatOption(Information::OctetStream);
          pdu2.addOptions(options1);
          int value = 0;
          Payload payload(sizeof(int), reinterpret_cast<const uint8_t*>(&value), Information::OctetStream);
          pdu2.setPayload(payload);
          auto handling2 = std::make_unique<HandlingExample>(std::move(pdu2.token().toBinaryConst()));
          if (manager.send(std::move(pdu2), std::move(handling2)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Put pdu failed!");
          }
          break;
        }
        case 80:  // key: P
        {
          /* Put消息数据包 */
          auto pdu2 = manager.createRequest(Information::Confirmable, Information::Put);
          Options options1;
          std::string path1 = "MOZARacing";
          std::string path2 = "ProductDevice";
          std::string path3 = "5853f8eb";
          std::string path4 = "AccOutDir";
          options1.insert(Information::UriPath, std::vector<uint8_t>(path1.begin(), path1.end()));
          options1.insert(Information::UriPath, std::vector<uint8_t>(path2.begin(), path2.end()));
          options1.insert(Information::UriPath, std::vector<uint8_t>(path3.begin(), path3.end()));
          options1.insert(Information::UriPath, std::vector<uint8_t>(path4.begin(), path4.end()));
          options1.insertContentFormatOption(Information::OctetStream);
          pdu2.addOptions(options1);
          int value = 1;
          Payload payload(sizeof(int), reinterpret_cast<const uint8_t*>(&value), Information::OctetStream);
          pdu2.setPayload(payload);
          auto handling2 = std::make_unique<HandlingExample>(std::move(pdu2.token().toBinaryConst()));
          if (manager.send(std::move(pdu2), std::move(handling2)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Put pdu failed!");
          }
          break;
        }
        case 100: // key: d
        {
          auto pdu2 = manager.createRequest(Information::NonConfirmable, Information::Post);
          Options options;
          options.insertURIOption("/MOZARacing/ProductDevice/050d36ef6f3930e4/CenterWheel");
          pdu2.addOptions(options);
          auto handling2 = std::make_unique<HandlingExample>(std::move(pdu2.token().toBinaryConst()));
          if (manager.send(std::move(pdu2), std::move(handling2)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Put pdu failed!");
          }
          break;
        }
        case 68:  // key: D
        {
          auto pdu2 = manager.createRequest(Information::NonConfirmable, Information::Put);
          Options options;
          options.insertURIOption("/MOZARacing/ProductDevice/050d36ef6f3930e4/HandsOffProtection");
          pdu2.addOptions(options);
          int value = 1;
          Payload payload(sizeof(int), reinterpret_cast<const uint8_t*>(&value), Information::OctetStream);
          pdu2.setPayload(payload);
          auto handling2 = std::make_unique<HandlingExample>(std::move(pdu2.token().toBinaryConst()));
          if (manager.send(std::move(pdu2), std::move(handling2)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Put pdu failed!");
          }
          break;
        }
        case 99: // key: c
        {
          auto pdu2 = manager.createRequest(Information::Confirmable, Information::Put);
          Options options1;
          options1.insertURIOption("/MOZARacing/ProductDevice/050d36ef6f3930e4/NaturalFriction");
          options1.insertOsberveOption(true);
          options1.insertContentFormatOption(Information::OctetStream);
          pdu2.addOptions(options1);
          int value = 50;
          Payload payload(sizeof(int), reinterpret_cast<const uint8_t*>(&value), Information::OctetStream);
          pdu2.setPayload(payload);
          auto handling2 = std::make_unique<HandlingExample>(std::move(pdu2.token().toBinaryConst()));
          if (manager.send(std::move(pdu2), std::move(handling2)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Put pdu failed!");
          }
          break;
        }
        case 67: // key: C
        {
          auto pdu2 = manager.createRequest(Information::Confirmable, Information::Put);
          Options options1;
          options1.insertURIOption("/MOZARacing/ProductDevice/050d36ef6f3930e4/LimitAngle");
          options1.insertContentFormatOption(Information::Cbor);
          pdu2.addOptions(options1);

          const char* cborHexData = "a26a4c696d6974416e676c6518967047616d654d6178696d756d416e676c6518c8";
          size_t length = std::strlen(cborHexData) / 2;
          std::vector<uint8_t> cborBytes(length);
          for (size_t i = 0; i < length; ++i) {
              std::sscanf(&cborHexData[i * 2], "%2hhx", &cborBytes[i]);
          }
          Payload payload((int)cborBytes.size(), reinterpret_cast<const uint8_t*>(cborBytes.data()), Information::Cbor);
          pdu2.setPayload(payload);
          auto handling2 = std::make_unique<HandlingExample>(std::move(pdu2.token().toBinaryConst()));
          if (manager.send(std::move(pdu2), std::move(handling2)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Put pdu failed!");
          }
          break;
        }
        case 115: // key: s
        {
          auto pdu2 = manager.createRequest(Information::Confirmable, Information::Put);
          Options options1;
          options1.insertURIOption("/MOZARacing/ProductDevice/dd0d38f77dfd/CluthPaddleAxisMode");
          options1.insertContentFormatOption(Information::OctetStream);
          pdu2.addOptions(options1);
          int value = 1;
          Payload payload(sizeof(int), reinterpret_cast<const uint8_t*>(&value), Information::OctetStream);
          pdu2.setPayload(payload);
          auto handling2 = std::make_unique<HandlingExample>(std::move(pdu2.token().toBinaryConst()));
          if (manager.send(std::move(pdu2), std::move(handling2)) == false)
          {
            Log::Logging(LOG_LEVEL::ERR, "send Put pdu failed!");
          }
          break;
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
