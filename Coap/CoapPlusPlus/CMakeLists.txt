cmake_minimum_required(VERSION 3.14.0)
project(libcoapcpp
  VERSION 1.0.0
  DESCRIPTION "基于libcoap的C++封装库"
  HOMEPAGE_URL "http://git.gudsen-rd.com/rs21/rs21-pc/module/libcoapcpp"
  LANGUAGES CXX)
set(PACKAGE_VERSION ${PROJECT_VERSION})
set(PACKAGE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(PACKAGE_URL ${PROJECT_HOMEPAGE_URL})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /std:c++latest")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "编译类型，默认是RelWithDebInfo" FORCE)
endif()

# option
option(ENABLE_TESTS "构建测试" OFF)
option(ENABLE_EXAMPLES "构建示例" ON)
option(ENABLE_DOCS "构建Doxygen文档" OFF)
option(MAKE_LIBRARY "将${PROJECT_NAME}模块编译成库" ON)
message(STATUS "构建测试: ${ENABLE_TESTS}")
message(STATUS "构建示例: ${ENABLE_EXAMPLES}")
message(STATUS "构建Doxygen文档: ${ENABLE_DOCS}")
message(STATUS "将${PROJECT_NAME}模块编译成库: ${MAKE_LIBRARY}")

include(CMakePrintHelpers) # CMakePrintHelpers模块 参见 https://cmake.org/cmake/help/latest/module/CMakePrintHelpers.html
cmake_print_variables(
  PROJECT_NAME
  PROJECT_VERSION
  CMAKE_BUILD_TYPE)

include(CMakeDependentOption) # CMakeDependentOption模块 参见 https://cmake.org/cmake/help/latest/module/CMakeDependentOption.html
cmake_dependent_option(
  MAKE_STATIC_LIBRARY "将${PROJECT_NAME}模块编译成静态库" ON
  "MAKE_LIBRARY" ON)

# 安装头文件
include(cmake/install_include_dir.cmake)
install_include_dir()

# Config
configure_file(${CMAKE_CURRENT_LIST_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# 源代码、头文件、QML文件
file(GLOB_RECURSE source_files "src/*.cc" "src/*.h" ${CMAKE_CURRENT_BINARY_DIR}/config.h)
file(GLOB_RECURSE header_files "include/*.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# add_sub
add_subdirectory(src)

# Enable
if(ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif(ENABLE_TESTS)

if(ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif(ENABLE_EXAMPLES)

if(ENABLE_DOCS)
  find_package(Doxygen)

  if(Doxygen_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(
      ${DOXYGEN_IN}
      ${DOXYGEN_OUT}
      @ONLY)

    add_custom_target(
      doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "用Doxygen生成API文档"
      VERBATIM)

    message(STATUS "设置Doxygen文档构建")
    install(
      DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc"
      TYPE DOC
    )
  else(Doxygen_FOUND)
    message(WARNING "需要安装Doxygen来生成Doxygen文档")
  endif(Doxygen_FOUND)
endif(ENABLE_DOCS)
