# Internal miwear CMake - not included in open source release
#
# nuttx_add_prebuilt 的工作原理：检查当前目录是否有源码文件。
# 如果有源码，则从源码编译生成库；如果没有源码，则使用 vendor 目录下的预编译库。
# 因此 miwear 的源码必须放在此目录下，而不是 shell/miwear/，
# 否则 nuttx_add_prebuilt 会因为找不到源码而使用预编译库，
# 导致与源码编译的 gui_wrapper 产生符号冲突（multiple definition）。

set(MIWEAR_LIB_NAME "vappframework")

# miwear source directory (sources live alongside this CMakeLists.txt)
set(MIWEAR_SRC_DIR ${CMAKE_CURRENT_LIST_DIR})
# shell common directory
set(SHELL_DIR ${CMAKE_CURRENT_LIST_DIR}/../../shell)

if(CONFIG_QUICKAPP)

  nuttx_include_directories_for_all_apps(${MIWEAR_SRC_DIR})

if(CONFIG_QUICKAPP_MIWEAR)

  nuttx_add_prebuilt(
    BIN_TARGETS
    ${MIWEAR_LIB_NAME}
  )

  if(CONFIG_APP_BAIDU_MAP)
    nuttx_add_prebuilt(EXPORT_FILES thirdparty/baidumap/libs)
  endif()

  nuttx_add_library(${MIWEAR_LIB_NAME} STATIC)

  if(CONFIG_QUICKAPP_BLUETOOTH_ENHANCE)
    set(QUICKAPP_BLUETOOTH_ENHANCE_SRC
        ${MIWEAR_SRC_DIR}/bt_enhance_handler.cpp
        ${MIWEAR_SRC_DIR}/bt_enhance_handler.h)
  endif(CONFIG_QUICKAPP_BLUETOOTH_ENHANCE)

  include(${SHELL_DIR}/common/CMakeLists.txt)

  file(GLOB PM_SRC ${MIWEAR_SRC_DIR}/pm/*.cpp
       ${MIWEAR_SRC_DIR}/pm/*.h)

  if(CONFIG_QUICKAPP_ENABLE_LMK)
  file(GLOB LMK_SRCS ${MIWEAR_SRC_DIR}/low_memorymanager/*.cpp
       ${MIWEAR_SRC_DIR}/low_memorymanager/*.h)
  endif(CONFIG_QUICKAPP_ENABLE_LMK)

  set(VAPP_LMK_TEST_NAME "lmk_test")
  # config lmk unit test
  if(CONFIG_QUICKAPP_LMK_TEST)
    nuttx_add_application(
      NAME
      ${VAPP_LMK_TEST_NAME}
      INCLUDE_DIRECTORIES
      ${NUTTX_APPS_DIR}/external/googletest/googletest/googletest/include
      ${NUTTX_APPS_DIR}/external/rapidjson/rapidjson/include/
      ${MIWEAR_SRC_DIR}/low_memorymanager
      ${CMAKE_CURRENT_LIST_DIR}/../../tests/testcase
      SRCS
      ${CMAKE_CURRENT_LIST_DIR}/../../tests/testcase/lmk_test/lmk_test.cpp
      ${MIWEAR_SRC_DIR}/low_memorymanager/appcount_policy.cpp
      ${MIWEAR_SRC_DIR}/low_memorymanager/memory_policy.cpp
      ${MIWEAR_SRC_DIR}/low_memorymanager/lmk_kvdb.cpp
      ${MIWEAR_SRC_DIR}/low_memorymanager/request_impl.cpp
      STACKSIZE
      ${CONFIG_QUICKAPP_STACKSIZE}
      PRIORITY
      ${CONFIG_QUICKAPP_PRIORITY})
  endif(CONFIG_QUICKAPP_LMK_TEST)

  if(CONFIG_QUICKAPP_PERMISSION_MANAGER)
    set(QUICKAPP_PERMISSION_MANAGER_SRC
        ${MIWEAR_SRC_DIR}/permission_impl.cpp)
    nuttx_include_directories_for_all_apps(
      ${NUTTX_APPS_DIR}/frameworks/security/permission)
  endif(CONFIG_QUICKAPP_PERMISSION_MANAGER)

  target_sources(
    ${MIWEAR_LIB_NAME}
    PRIVATE ${MIWEAR_SRC_DIR}/app_manager_impl.cpp
            ${MIWEAR_SRC_DIR}/lmk_impl.cpp
            ${QUICKAPP_PERMISSION_MANAGER_SRC}
            ${MIWEAR_SRC_DIR}/qwrapper.cpp
            ${SHELL_COMMON_SRCS}
            ${PM_SRC}
            ${QUICKAPP_BLUETOOTH_ENHANCE_SRC}
            ${LMK_SRCS})

  target_include_directories(
    ${MIWEAR_LIB_NAME}
    PUBLIC ${MIWEAR_SRC_DIR}/low_memorymanager ${SHELL_COMMON_INCLUDES}
           ${NUTTX_APPS_DIR}/frameworks/runtimes/ash/c/include)

  target_compile_definitions(${MIWEAR_LIB_NAME} PUBLIC -DAIOTJS_HAVE_GUI)

  nuttx_link_libraries(${MIWEAR_LIB_NAME} quickapp
                       quickapp_inspector cash)

  if(CONFIG_MICROSERVICE)
    target_sources(${MIWEAR_LIB_NAME} PRIVATE ${MIWEAR_SRC_DIR}/microservice_package_observer.cpp)
    nuttx_link_libraries(${MIWEAR_LIB_NAME} microservice_clientapi)
  endif(CONFIG_MICROSERVICE)
endif(CONFIG_QUICKAPP_MIWEAR)

endif(CONFIG_QUICKAPP)
