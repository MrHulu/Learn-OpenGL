cmake_minimum_required(VERSION 3.1.8)
project(libcoap-example VERSION 0.1.0)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(UTILS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)

find_package(OpenSSL REQUIRED)
find_package(libcoap REQUIRED)
enable_testing()

#utils
file(GLOB_RECURSE utils_files "utils/*.cc" "utils/*.h")
add_library(Utils STATIC)
target_sources(Utils PRIVATE ${utils_files})
set_target_properties(Utils PROPERTIES CXX_STANDARD 20)
target_include_directories(Utils PRIVATE libcoap::coap-3)
target_link_libraries(Utils PRIVATE libcoap::coap-3)

add_subdirectory(Client)
add_subdirectory(Server)



