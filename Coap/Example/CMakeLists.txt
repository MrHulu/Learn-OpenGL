cmake_minimum_required(VERSION 3.20.0)
project(libcoap-example VERSION 0.1.0)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fabi-version=0 -fno-leading-underscore")
#set(CMAKE_C_STANDARD 99)
#set(CMAKE_C_STANDARD_REQUIRED ON)

option(ENABLE_TESTS "构建测试" ON)
 
find_package(OpenSSL REQUIRED)
find_package(libcoap REQUIRED)

#utils
file(GLOB_RECURSE utils_files "utils/*.cc" "utils/*.h") 
add_library(Utils STATIC)
target_sources(Utils PRIVATE ${utils_files}) 
set_target_properties(Utils PROPERTIES CXX_STANDARD 20 INCLUDE_CURRENT_DIR ON)
target_include_directories(Utils PRIVATE libcoap::coap-3)
target_include_directories(Utils PUBLIC "${PROJECT_SOURCE_DIR}/utils/")
target_link_libraries(Utils PRIVATE libcoap::coap-3)

add_subdirectory(Client)
add_subdirectory(Server)
 
if (ENABLE_TESTS)
  message(STATUS "启用测试")
  enable_testing()
  add_subdirectory(tests)
endif (ENABLE_TESTS)


