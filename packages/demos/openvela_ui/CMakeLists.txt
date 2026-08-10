# ##############################################################################
# Native openvela smart-band UI demo
# ##############################################################################

if(CONFIG_LVX_USE_DEMO_OPENVELA_UI)
  set(PROGNAME openvela_ui)
  set(PRIORITY 100)
  set(STACKSIZE 65536)
  set(MODULE ${CONFIG_LVX_USE_DEMO_OPENVELA_UI})
  set(MAINSRC openvela_ui_main.c)
  set(UI_SRCS openvela_ui.c openvela_ui_power.c openvela_ui_weather.c
              openvela_ui_city_data.c openvela_ui_sport.c
              openvela_ui_sync.c)

  if(CONFIG_OPENVELA_UI_HTTP_TIMESYNC)
    list(APPEND UI_SRCS openvela_ui_timesync.c)
  endif()

  nuttx_add_application(
    NAME ${PROGNAME}
    PRIORITY ${PRIORITY}
    STACKSIZE ${STACKSIZE}
    MODULE ${MODULE}
    SRCS ${MAINSRC} ${UI_SRCS}
  )
endif()
