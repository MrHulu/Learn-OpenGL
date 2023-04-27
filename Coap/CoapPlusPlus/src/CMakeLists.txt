cmake_minimum_required(VERSION 3.20.0)

find_package(libcoap REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)

# 编译二进制版本
add_library(${PROJECT_NAME}-objs OBJECT)
target_sources(${PROJECT_NAME}-objs 
  PRIVATE
    ${source_files})
set_target_properties(${PROJECT_NAME}-objs
  PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    INCLUDE_CURRENT_DIR ON
    DEBUG_POSTFIX "_d")
target_include_directories(${PROJECT_NAME}-objs
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}-objs
  PUBLIC
    libcoap::coap-3  
    OpenSSL::SSL)
add_compile_options(${PROJECT_NAME}-objs
  PRIVATE 
    ${CXX_FLAGS}
    $<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>
    $<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>
    $<$<CONFIG:Release>:${CXX_FLAGS_RELWITHDEBINFO}>)

# 编译静态库
if(MAKE_STATIC_LIBRARY)
  add_library(${PROJECT_NAME}-static
    STATIC
      $<TARGET_OBJECTS:${PROJECT_NAME}-objs>)
  set_target_properties(${PROJECT_NAME}-static
    PROPERTIES
      DEBUG_POSTFIX "_d"
       OUTPUT_NAME ${PROJECT_NAME})
endif()

install(
  TARGETS
    ${PROJECT_NAME}-static
    # ${PROJECT_NAME}-shared
  ARCHIVE
    DESTINATION ${INSTALL_LIBDIR}
    COMPONENT lib
  RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
  LIBRARY
    DESTINATION ${INSTALL_LIBDIR}
    COMPONENT lib
)

# target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR}/utils)
# target_link_libraries(server PRIVATE libcoap::coap-3 OpenSSL::SSL Utils)
# set_target_properties(server PROPERTIES CXX_STANDARD 14)
 