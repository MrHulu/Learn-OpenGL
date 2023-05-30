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

add_executable(tst_DataStruct
  "tst_DataStruct.cc"
  )
set_target_properties(tst_DataStruct 
  PROPERTIES
    AUTOUIC ON
    AUTOMOC ON
    AUTORCC ON
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    INCLUDE_CURRENT_DIR ON)
target_include_directories(tst_DataStruct
  PRIVATE
    ${PROJECT_NAME}-objs
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(tst_DataStruct 
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt5::Core
    libcoap::coap-3
    ${PROJECT_NAME}-objs)

add_test(NAME tst_DataStruct COMMAND  tst_DataStruct)
