if(CONFIG_QUICKAPP_FOLME_ANIMENGINE_ADAPTER)

  nuttx_add_prebuilt(
    BIN_TARGETS
    folme_feature
  )

  nuttx_add_library(folme_feature STATIC)

  list(
    APPEND 
    FEATURE_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/folme_impl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/widget_animengine_adapter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/widget_animengine_register.cpp)

  set(JIDL_PATHS ${CMAKE_CURRENT_LIST_DIR}/folme.jidl)
  target_include_directories(
    folme_feature
    PRIVATE
    ${NUTTX_APPS_DIR}/frameworks/graphics/animengine/include
    ${NUTTX_APPS_DIR}/frameworks/runtimes/feature/src
    ${NUTTX_APPS_DIR}/frameworks/runtimes/quickapp/src
    ${NUTTX_APPS_DIR}/frameworks/runtimes/quickapp/src/gui
    ${NUTTX_APPS_DIR}/external/yoga/yoga
    ${NUTTX_APPS_DIR}/frameworks/runtimes/quickapp/src/framework
    ${NUTTX_APPS_DIR}/frameworks/runtimes/quickapp/src/jse/quickjs
    ${NUTTX_APPS_DIR}/frameworks/runtimes/quickapp/src/jse
    ${NUTTX_APPS_DIR}/frameworks/runtimes/quickapp/src/jse/modules/system
    ${NUTTX_APPS_DIR}/external/rapidjson/rapidjson/include
    ${NUTTX_APPS_DIR}/frameworks/runtimes/ash/include
    ${CMAKE_BINARY_DIR})

  nuttx_add_jidl(
    TARGET
    folme_feature
    FEATURE_SRCS
    ${FEATURE_SRCS}
    JIDLS
    ${JIDL_PATHS}
    FEATURE_NAMES
    system_folme)

  # propagate quickapp log level
  if(DEFINED CONFIG_QUICKAPP_LOG_LEVEL)
    target_compile_definitions(folme_feature PRIVATE ATOTJS_LOG_LEVEL=${CONFIG_QUICKAPP_LOG_LEVEL})
  endif()

endif(CONFIG_QUICKAPP_FOLME_ANIMENGINE_ADAPTER)