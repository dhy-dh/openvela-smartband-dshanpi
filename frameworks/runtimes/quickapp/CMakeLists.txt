# 鉴于cmake和makefile会在一个阶段共存，所以只做Makefile翻译，不做模块拆分
# quickapp的代码模块化做的是很差的，现在拆分涉及到代码结构调整，建议quickapp重构时去拆模块

set(CMAKE_C_STANDARD 11) # C11
set(CMAKE_CXX_STANDARD 17) # C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ########### variables #################
# set shared source, includes and libs
set(QUICKAPP_COMMON_SRCS
    ""
    CACHE INTERNAL "")
set(QUICKAPP_COMMON_INCLUDES
    ""
    CACHE INTERNAL "")
set(QUICKAPP_COMMON_LIBS
    ""
    CACHE INTERNAL "")

# set AIOTJS_HAVE_GUI so that jse_new_prompt.cpp can be compiled
set(AIOTJS_HAVE_GUI
    1
    CACHE INTERNAL "") # todo: remove this global setting
# support compiling source file containing macro defination DAIOTJS_HAVE_GUI
add_definitions(-DAIOTJS_HAVE_GUI) # todo: remove this global macro definations

# global macro definations
add_definitions(-DAIOTJS_PLATFORM="Vela")

# project names
set(VAPP_NAME "vapp")
set(VAPP_XMS_NAME "vappxms")

if(CONFIG_QUICKAPP)

  # declare prebuilt libraries and header files
  nuttx_add_prebuilt(
    BIN_TARGETS
    gui_wrapper
    quickapp
    quickappfeatures
    quickapp_inspector
    APP_TARGETS
    ${VAPP_NAME}
    ${VAPP_XMS_NAME}
    EXPORT_FILES
    include/
    shell/
    README.md
    README_en.md
    docs/
    GLOB_REGEX
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp # it is source env, if directory './src/' has any cpp file
  )

  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-alloc-size-larger-than>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-array-bounds>)

  nuttx_include_directories_for_all_apps(${CMAKE_CURRENT_LIST_DIR}/include)

  # main source lib
  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/src/)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/)
  endif()


  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/modules/)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modules/)
  endif()

  if(CONFIG_QJS_EXTENDS AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/qjsextends/)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/qjsextends)
  endif()

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/inspector/)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/inspector)
  endif()

  if(CONFIG_WASM_FEATURE_FRAMEWORK AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/wasm/)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/wasm)
  endif()

  set(AIOTJSC_PATH
      ${CMAKE_CURRENT_BINARY_DIR}
      CACHE INTERNAL "")

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/tools/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/tools/CMakeLists.txt)
  endif()

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/src/)
    # mark generated protobuf files so CMake doesn't check existence at configure time
    set_source_files_properties(
      ${CMAKE_BINARY_DIR}/vdom.pb-c.c
      ${CMAKE_BINARY_DIR}/vdom.pb-c.h
      PROPERTIES GENERATED TRUE)

    nuttx_add_library(quickapp STATIC
      ${QUICKAPP_COMMON_SRCS}
    )

    target_include_directories(quickapp PUBLIC
      ${QUICKAPP_COMMON_INCLUDES}
    )

    nuttx_link_libraries(quickapp
      gui_wrapper quickapp_inspector
    )
  endif()

  include(${CMAKE_CURRENT_LIST_DIR}/shell/CMakeLists.txt)

  # include proprietary configurations if available (not included in open source release)
  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/proprietary/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/proprietary/CMakeLists.txt) # option
  endif()

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/tests/CMakeLists.txt)
  endif()

  # aiotjs log
  if(DEFINED CONFIG_QUICKAPP_LOG_LEVEL)
    add_definitions(-DATOTJS_LOG_LEVEL=${CONFIG_QUICKAPP_LOG_LEVEL})
  endif()

endif(CONFIG_QUICKAPP)
