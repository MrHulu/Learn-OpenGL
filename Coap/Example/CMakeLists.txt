cmake_minimum_required(VERSION 3.1.8)
project(libcoap-example VERSION 0.1.0)


find_package(OpenSSL REQUIRED)
find_package(libcoap REQUIRED)
enable_testing()

add_subdirectory(Client)
add_subdirectory(Server)



