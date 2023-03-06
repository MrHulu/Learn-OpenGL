#include "server.h"

#include <iostream>

int main()
{
    std::cout << "app start..." <<std::endl;
    try {
        SimpleServer server;
        //server.addTextResource("ok");
        server.start(1000);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        server.stop();
    }
    catch(std::exception& e) {
        std::cout << e.what() <<std::endl;
        return -1;
    }
    std::cout << "app stop..." <<std::endl;
    return 0;
}