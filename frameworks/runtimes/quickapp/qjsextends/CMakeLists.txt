if(CONFIG_QJS_EXTENDS)
  if(CONFIG_QJS_EXTENDS_LOG_LEVEL)
    add_definitions(-DQJS_EXTENDS_LOG_LEVEL=${CONFIG_QJS_EXTENDS_LOG_LEVEL})
  endif()

  set(APPDIR ${CMAKE_CURRENT_LIST_DIR}/../../../..)
  set(QUICKJS_DIR ${APPDIR}/interpreters/quickjs/quickjs)
  set(QJS_EXTENDS_LIB_NAME "libqjsextends")

  set(QJS_EXTENDS_SOURCES
      ${CMAKE_CURRENT_LIST_DIR}/jshelp/js_utils.cpp
      ${CMAKE_CURRENT_LIST_DIR}/jshelp/log_util.cpp
      ${CMAKE_CURRENT_LIST_DIR}/jshelp/module_js_impl.cpp)

  nuttx_add_prebuilt(
    BIN_TARGETS
    ${QJS_EXTENDS_LIB_NAME})

  nuttx_add_library(${QJS_EXTENDS_LIB_NAME} STATIC
    ${QJS_EXTENDS_SOURCES}
  )
  nuttx_include_directories()
  target_include_directories(${QJS_EXTENDS_LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

endif()
