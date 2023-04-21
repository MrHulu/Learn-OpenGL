cmake_minimum_required(VERSION 3.20.0)

# Qt
file(GLOB_RECURSE widget_files "widget/*.cc" "widget/*.h" "widget/*.ui")
find_package(QT NAMES Qt6 Qt5 Core COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(libcoap REQUIRED CONFIG)

# 使用了AUTOUIC AUTOMOC AUTORCC 就必须让libcoap：：coap-3显式设置IMPORTED_LOCATION
set_target_properties(libcoap::coap-3 PROPERTIES IMPORTED_LOCATION ${libcoap_DIR})

file(GLOB_RECURSE client_files "./*.cc" "./*.h")
add_executable(client
    ${widget_files}
    ${client_files}
)
target_include_directories(client PRIVATE Qt5::Core Qt5::Widgets ${PROJECT_SOURCE_DIR}/utils)
target_link_libraries(client PRIVATE libcoap::coap-3 OpenSSL::SSL Qt5::Core Qt5::Widgets Utils)
set_target_properties(client PROPERTIES 
    CXX_STANDARD 20
    AUTOUIC ON
    AUTOMOC ON
    AUTORCC ON
)

