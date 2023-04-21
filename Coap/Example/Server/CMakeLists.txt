cmake_minimum_required(VERSION 3.20.0)

file(GLOB_RECURSE server_files "./*.cc" "./*.h")
add_executable(server  ${server_files})
target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR}/utils)
target_link_libraries(server PRIVATE libcoap::coap-3 OpenSSL::SSL Utils)
set_target_properties(server PROPERTIES CXX_STANDARD 14)
