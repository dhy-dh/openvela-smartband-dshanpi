# quickapp xms
if(CONFIG_QUICKAPP_VAPP_XMS)

include(${CMAKE_CURRENT_LIST_DIR}/../common/CMakeLists.txt)

nuttx_add_application(
    NAME
    ${VAPP_XMS_NAME}
    DEFINITIONS
    -DAIOTJS_HAVE_GUI
    INCLUDE_DIRECTORIES
    ${SHELL_COMMON_INCLUDES}
    SRCS
    ${CMAKE_CURRENT_LIST_DIR}/quick_application.cpp
    ${CMAKE_CURRENT_LIST_DIR}/quick_activity.cpp
    ${CMAKE_CURRENT_LIST_DIR}/qwrapper.cpp
    ${SHELL_COMMON_SRCS}
    STACKSIZE
    ${CONFIG_QUICKAPP_STACKSIZE}
    PRIORITY
    ${CONFIG_QUICKAPP_PRIORITY}
    DEPENDS
    ${QUICKAPP_COMMON_LIBS}
    quickapp
    cash
    gui_wrapper
    lvgl)
endif(CONFIG_QUICKAPP_VAPP_XMS)
