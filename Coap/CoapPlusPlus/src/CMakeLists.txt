cmake_minimum_required(VERSION 3.14.0)

find_package(libcoap REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)
if(libcoap_FOUND)
  message(STATUS "Found libcoap version ${libcoap_VERSION}, minimum_required is 4.3.4")
else()
  message(FATAL_ERROR "libcoap_FOUND not found")
endif()

include(GNUInstallDirs)
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} )
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR} )
set(INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# 编译二进制版本
add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} 
  PRIVATE
    ${source_files})
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    INCLUDE_CURRENT_DIR ON
    DEBUG_POSTFIX "_d")
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    libcoap::coap-3  
    OpenSSL::SSL)
add_compile_options(${PROJECT_NAME}
  PRIVATE 
    ${CXX_FLAGS}
    $<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>
    $<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>
    $<$<CONFIG:Release>:${CXX_FLAGS_RELWITHDEBINFO}>)


install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets 
  ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT lib
  LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT lib
  RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
)
install(
  EXPORT ${PROJECT_NAME}Targets
  DESTINATION ${INSTALL_CMAKEDIR}
  COMPONENT dev
)
include (CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${INSTALL_CMAKEDIR}
)
install(
  FILES 
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${INSTALL_CMAKEDIR}
  COMPONENT dev
)
