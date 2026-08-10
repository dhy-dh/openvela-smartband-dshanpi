
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/products/)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/products/)
endif()


if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/app/
   OR EXISTS ${CMAKE_CURRENT_LIST_DIR}/router/)
  nuttx_add_library(quickappfeatures STATIC)
  target_include_directories(quickappfeatures PRIVATE ${NUTTX_APPS_DIR}/frameworks/runtimes/feature/include)

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/app/)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/app)
  endif()

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/router/)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/router)
  endif()
endif()

  nuttx_add_prebuilt(
    BIN_TARGETS
    folme_feature
  )

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/folme/)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/folme)
endif()

# propagate quickapp log level to feature targets
if(DEFINED CONFIG_QUICKAPP_LOG_LEVEL AND CONFIG_FEATURE_FRAMEWORK)
  if(TARGET feature_impls)
    target_compile_definitions(feature_impls PRIVATE ATOTJS_LOG_LEVEL=${CONFIG_QUICKAPP_LOG_LEVEL})
  endif()
endif()