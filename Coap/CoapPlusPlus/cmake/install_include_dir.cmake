function(install_include_dir)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs)
  cmake_parse_arguments(install_include_dir
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN})
  # 调用install_include_dir()的project的目录结构必须按照下面提供
  set(install_include_dir_SOURCE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
  set(install_include_dir_BINARY_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include")

  file(
    GLOB_RECURSE
    source_include_file_paths
    "${install_include_dir_SOURCE_INCLUDE_DIR}/*.h")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory "${install_include_dir_BINARY_INCLUDE_DIR}"
  )

  set(binary_include_file_paths)
  
  foreach (source_include_file_path ${source_include_file_paths})
    # 读取仓库中include下的头文件内容
    file(
      READ "${source_include_file_path}"
      source_include_file_data
    )
    # 从内容中获取src下的头文件的相对路径
    string(
      REGEX REPLACE
        "#include \"(.*)\""
        "\\1"
      source_src_file_relative_path_from_source_include_file_dir
      "${source_include_file_data}"
    )
    # 相对于哪个include头文件的路径
    cmake_path(
      GET source_include_file_path
      PARENT_PATH source_include_file_dir
    )
    # 得到仓库中src的头文件的绝对路径
    cmake_path(
      ABSOLUTE_PATH source_src_file_relative_path_from_source_include_file_dir
      BASE_DIRECTORY "${source_include_file_dir}"
      OUTPUT_VARIABLE source_src_file_path
    )
    cmake_path(
      RELATIVE_PATH source_include_file_path
      BASE_DIRECTORY "${install_include_dir_SOURCE_INCLUDE_DIR}"
      OUTPUT_VARIABLE source_include_file_relative_path
    )
    cmake_path(
      ABSOLUTE_PATH source_include_file_relative_path
      BASE_DIRECTORY "${install_include_dir_BINARY_INCLUDE_DIR}"
      OUTPUT_VARIABLE binary_include_file_path
    )
    cmake_path(
      GET binary_include_file_path
      PARENT_PATH binary_include_file_dir
    )
    # 拷贝 仓库中src的头文件 到 构建中include的头文件
    add_custom_command(
      OUTPUT
        ${binary_include_file_path}
      COMMAND
        ${CMAKE_COMMAND} -E make_directory "${binary_include_file_dir}"
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different "${source_src_file_path}" "${binary_include_file_path}"
      DEPENDS
        ${source_src_file_path}
    )
    # TODO 考虑在target_sources()使用FILE_SET把头文件添加进来，需用到CMAKE 3.23
    list(APPEND binary_include_file_paths ${binary_include_file_path})
  endforeach ()

  add_custom_target(
    build_include_dir ALL
    DEPENDS ${binary_include_file_paths}
  )
  install(
    DIRECTORY "${install_include_dir_BINARY_INCLUDE_DIR}/"
    TYPE INCLUDE
  )
endfunction()