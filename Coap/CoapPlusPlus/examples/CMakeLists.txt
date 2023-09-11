cmake_minimum_required(VERSION 3.14.0)

find_package(libcoap REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)

# server
add_executable(server  
  server.cc
  ResourceInterfaceExample.h
)
target_compile_features(server PRIVATE cxx_std_20)
target_include_directories(server
PRIVATE
${PROJECT_NAME}
${PROJECT_SOURCE_DIR}/include)
target_link_libraries(server 
PRIVATE
libcoap::coap-3
${PROJECT_NAME})

# client 
add_executable(client  
  client.cc
  HandlingExample.h
)
target_compile_features(client PRIVATE cxx_std_20)
target_include_directories(client
  PRIVATE
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(client 
  PRIVATE
    libcoap::coap-3
    ${PROJECT_NAME})




