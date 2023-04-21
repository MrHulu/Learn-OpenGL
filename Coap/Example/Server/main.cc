#include "server.h"

#include <iostream>

int main()
{
    std::cout << "app start..." <<std::endl;
    try {
        SimpleServer server;
        //server.addTextResource("ok");
        server.start(1000);
        
        /* 测试是否可以被stop */
        //std::this_thread::sleep_for(std::chrono::seconds(5));
        server.stop();

    }
    catch(std::exception& e) {
        std::cout << e.what() <<std::endl;
        return -1;
    }
    
    //while(1); // 阻塞
    std::cout << "app stop..." <<std::endl;
    return 0;
}