cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(QT
  NAMES
    Qt6 Qt5
  REQUIRED COMPONENTS
    Test)
find_package(Qt${QT_VERSION_MAJOR}
  REQUIRED COMPONENTS
    Test)

add_executable(tst_Communication
  "tst_Communication.cc"
  )
target_sources(tst_Communication PRIVATE
  ${CMAKE_SOURCE_DIR}/Client/Client.cc
  ${CMAKE_SOURCE_DIR}/Client/Client.h
  ${CMAKE_SOURCE_DIR}/Server/Server.cc
  ${CMAKE_SOURCE_DIR}/Server/Server.h
  ${CMAKE_SOURCE_DIR}/Server/Resource.cc
  ${CMAKE_SOURCE_DIR}/Server/Resource.h
  ${CMAKE_SOURCE_DIR}/Server/TextResource.cc
  ${CMAKE_SOURCE_DIR}/Server/TextResource.h
)
set_target_properties(tst_Communication 
  PROPERTIES
    AUTOUIC ON
    AUTOMOC ON
    AUTORCC ON
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    INCLUDE_CURRENT_DIR ON)
target_include_directories(tst_Communication
  PRIVATE
   ${CMAKE_SOURCE_DIR}/Server
   ${CMAKE_SOURCE_DIR}/Client
   ${CMAKE_SOURCE_DIR}/utils)
target_link_libraries(tst_Communication 
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    libcoap::coap-3
    Utils)

add_test(NAME tst_Communication COMMAND  tst_Communication)
