cmake_minimum_required(VERSION 3.1.8)

add_executable(server 
    main.cc 
    server.h
    server.cc 
)
target_include_directories(server PRIVATE libcoap::coap-3)
target_link_libraries(server PRIVATE libcoap::coap-3 OpenSSL::SSL)
set_target_properties(server PROPERTIES CXX_STANDARD 14)