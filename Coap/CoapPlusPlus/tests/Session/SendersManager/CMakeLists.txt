cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(QT
  NAMES
    Qt6 Qt5 Core
  REQUIRED COMPONENTS
    Test)
find_package(Qt${QT_VERSION_MAJOR} 
  REQUIRED COMPONENTS
    Test)
find_package(libcoap REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)

add_executable(tst_SendersManager
  "tst_SendersManager.cc"
  "TestHandling.h"
  )
set_target_properties(tst_SendersManager 
  PROPERTIES
    AUTOUIC ON
    AUTOMOC ON
    AUTORCC ON
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    INCLUDE_CURRENT_DIR ON)
target_include_directories(tst_SendersManager
  PRIVATE
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(tst_SendersManager 
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt5::Core
    libcoap::coap-3
    ${PROJECT_NAME})

add_test(NAME tst_SendersManager COMMAND  tst_SendersManager)
