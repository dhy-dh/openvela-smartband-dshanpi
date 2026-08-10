file(
  GLOB
  SHELL_COMMON_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/*.h
  ${CMAKE_CURRENT_LIST_DIR}/permission/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/permission/*.h)

if(CONFIG_QUICKAPP_INPUT_METHOD)
  list(APPEND SHELL_COMMON_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/input_method/input_method_manager.cpp)
endif()

set(SHELL_COMMON_INCLUDES
    ${NUTTX_APPS_DIR}/external/rapidjson/rapidjson/include/ # for splash screen
    ${NUTTX_APPS_DIR}/frameworks/runtimes/ash/c/include
    ${CMAKE_CURRENT_LIST_DIR}/../ # for self
)
