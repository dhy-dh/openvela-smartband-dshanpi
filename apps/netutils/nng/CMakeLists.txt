#
# Copyright (C) 2024 Xiaomi Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#

if(CONFIG_NETUTILS_NNG)

  # ############################################################################
  # Config and Fetch NNG lib
  # ############################################################################

  set(NNG_DIR ${CMAKE_CURRENT_LIST_DIR}/nng)

  if(NOT EXISTS ${NNG_DIR})
    set(NNG_URL "https://github.com/nanomsg/nng/archive")
    FetchContent_Declare(
      nng_fetch
      URL ${NNG_URL}/v${CONFIG_NETUTILS_NNG_VERSION}.tar.gz SOURCE_DIR
          ${NNG_DIR} BINARY_DIR ${CMAKE_BINARY_DIR}/apps/netutils/nng/nng
      DOWNLOAD_NO_PROGRESS true
      TIMEOUT 30)

    FetchContent_GetProperties(nng_fetch)

    if(NOT nng_fetch)
      FetchContent_Populate(nng_fetch)
    endif()

    execute_process(COMMAND sh -c "patch -p0 < nng_porting_for_nuttx.patch"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  endif()

  set(CFLAGS
      -DNNG_PLATFORM_POSIX
      -DNNG_HAVE_GETRANDOM
      -DNNG_TRANSPORT_INPROC
      -DNNG_TRANSPORT_IPC
      -DNNG_TRANSPORT_TCP
      -DNNG_TRANSPORT_TLS
      -DNNG_TRANSPORT_WS
      -DNNG_TRANSPORT_WSS
      -DNNG_SETSTACKSIZE
      -DNNG_HAVE_PTHREAD_SETNAME_NP
      -DNNG_USE_EVENTFD
      -Wno-unused-value
      -Wno-shadow)

  if(CONFIG_SMP_NCPUS)
    list(APPEND CFLAGS -DNNG_MAX_TASKQ_THREADS=${CONFIG_SMP_NCPUS})
  else()
    list(APPEND CFLAGS -DNNG_MAX_TASKQ_THREADS=2)
  endif()
  if(CONFIG_NETUTILS_NNG_HAVE_EPOLL)
    list(APPEND CFLAGS -DNNG_HAVE_EPOLL -DNNG_HAVE_EPOLL_CREATE1)
  endif()
  if(CONFIG_NETUTILS_NNG_USE_MBEDTLS)
    list(APPEND CFLAGS -DNNG_TLS_ENGINE_INIT=nng_tls_engine_init_mbed
         -DNNG_TLS_ENGINE_FINI=nng_tls_engine_fini_mbed -DNNG_SUPP_TLS)
  endif()

  set(CSRCS
      ${NNG_DIR}/src/nng.c
      ${NNG_DIR}/src/nng_legacy.c
      ${NNG_DIR}/src/core/aio.c
      ${NNG_DIR}/src/core/device.c
      ${NNG_DIR}/src/core/dialer.c
      ${NNG_DIR}/src/core/file.c
      ${NNG_DIR}/src/core/idhash.c
      ${NNG_DIR}/src/core/init.c
      ${NNG_DIR}/src/core/list.c
      ${NNG_DIR}/src/core/listener.c
      ${NNG_DIR}/src/core/lmq.c
      ${NNG_DIR}/src/core/message.c
      ${NNG_DIR}/src/core/msgqueue.c
      ${NNG_DIR}/src//core/nng_global.c
      ${NNG_DIR}/src/core/options.c
      ${NNG_DIR}/src/core/pollable.c
      ${NNG_DIR}/src/core/panic.c
      ${NNG_DIR}/src/core/pipe.c
      ${NNG_DIR}/src/core/protocol.c
      ${NNG_DIR}/src/core/reap.c
      ${NNG_DIR}/src/core/socket.c
      ${NNG_DIR}/src/core/stats.c
      ${NNG_DIR}/src/core/stream.c
      ${NNG_DIR}/src/core/strs.c
      ${NNG_DIR}/src/core/taskq.c
      ${NNG_DIR}/src/core/tcp.c
      ${NNG_DIR}/src/core/thread.c
      ${NNG_DIR}/src/core/timer.c
      ${NNG_DIR}/src/core/url.c
      ${NNG_DIR}/src/platform/posix/posix_alloc.c
      ${NNG_DIR}/src/platform/posix/posix_atomic.c
      ${NNG_DIR}/src/platform/posix/posix_clock.c
      ${NNG_DIR}/src/platform/posix/posix_debug.c
      ${NNG_DIR}/src/platform/posix/posix_file.c
      ${NNG_DIR}/src/platform/posix/posix_ipcconn.c
      ${NNG_DIR}/src/platform/posix/posix_ipcdial.c
      ${NNG_DIR}/src/platform/posix/posix_ipclisten.c
      ${NNG_DIR}/src/platform/posix/posix_pipe.c
      ${NNG_DIR}/src/platform/posix/posix_resolv_gai.c
      ${NNG_DIR}/src/platform/posix/posix_sockaddr.c
      ${NNG_DIR}/src/platform/posix/posix_tcpconn.c
      ${NNG_DIR}/src/platform/posix/posix_tcpdial.c
      ${NNG_DIR}/src/platform/posix/posix_tcplisten.c
      ${NNG_DIR}/src/platform/posix/posix_thread.c
      ${NNG_DIR}/src/platform/posix/posix_udp.c
      ${NNG_DIR}/src/platform/posix/posix_rand_getrandom.c
      ${NNG_DIR}/src/sp/transport.c
      ${NNG_DIR}/src/sp/protocol/bus0/bus.c
      ${NNG_DIR}/src/sp/protocol/pair0/pair.c
      ${NNG_DIR}/src/sp/protocol/pair1/pair.c
      ${NNG_DIR}/src/sp/protocol/pair1/pair1_poly.c
      ${NNG_DIR}/src/sp/protocol/pipeline0/pull.c
      ${NNG_DIR}/src/sp/protocol/pipeline0/push.c
      ${NNG_DIR}/src/sp/protocol/pubsub0/pub.c
      ${NNG_DIR}/src/sp/protocol/pubsub0/sub.c
      ${NNG_DIR}/src/sp/protocol/pubsub0/xsub.c
      ${NNG_DIR}/src/sp/protocol/reqrep0/rep.c
      ${NNG_DIR}/src/sp/protocol/reqrep0/req.c
      ${NNG_DIR}/src/sp/protocol/reqrep0/xrep.c
      ${NNG_DIR}/src/sp/protocol/reqrep0/xreq.c
      ${NNG_DIR}/src/sp/protocol/survey0/respond.c
      ${NNG_DIR}/src/sp/protocol/survey0/survey.c
      ${NNG_DIR}/src/sp/protocol/survey0/xrespond.c
      ${NNG_DIR}/src/sp/protocol/survey0/xsurvey.c
      ${NNG_DIR}/src/sp/transport/inproc/inproc.c
      ${NNG_DIR}/src/sp/transport/ipc/ipc.c
      ${NNG_DIR}/src/sp/transport/tcp/tcp.c
      ${NNG_DIR}/src/sp/transport/tls/tls.c
      ${NNG_DIR}/src/sp/transport/ws/websocket.c
      ${NNG_DIR}/src/supplemental/base64/base64.c
      ${NNG_DIR}/src/supplemental/http/http_chunk.c
      ${NNG_DIR}/src/supplemental/http/http_client.c
      ${NNG_DIR}/src/supplemental/http/http_conn.c
      ${NNG_DIR}/src/supplemental/http/http_msg.c
      ${NNG_DIR}/src/supplemental/http/http_public.c
      ${NNG_DIR}/src/supplemental/http/http_schemes.c
      ${NNG_DIR}/src/supplemental/http/http_server.c
      ${NNG_DIR}/src/supplemental/sha1/sha1.c
      ${NNG_DIR}/src/supplemental/tls/tls_common.c
      ${NNG_DIR}/src/supplemental/util/platform.c
      ${NNG_DIR}/src/supplemental/websocket/websocket.c)

  if(CONFIG_NETUTILS_NNG_HAVE_EPOLL)
    list(APPEND CSRCS ${NNG_DIR}/src/platform/posix/posix_pollq_epoll.c)
  else()
    list(APPEND CSRCS ${NNG_DIR}/src/platform/posix/posix_pollq_poll.c)
  endif()
  if(CONFIG_NETUTILS_NNG_USE_MBEDTLS)
    list(APPEND CSRCS ${NNG_DIR}/src/supplemental/tls/mbedtls/tls.c)
  endif()

  set_property(
    TARGET nuttx
    APPEND
    PROPERTY NUTTX_INCLUDE_DIRECTORIES ${NNG_DIR}/include)
  set(INCDIR ${NNG_DIR}/src)

  if(CONFIG_NETUTILS_NNG_TEST)
    set(TESTDIR ${NNG_DIR}/tests)

    list(
      APPEND
      CFLAGS
      -DNNG_HAVE_PAIR0
      -DNNG_HAVE_BUS0
      -DNNG_HAVE_PULL0
      -DNNG_HAVE_PUSH0
      -DNNG_HAVE_PUB0
      -DNNG_HAVE_SUB0
      -DNNG_HAVE_REQ0
      -DNNG_HAVE_REP0
      -DNNG_HAVE_SURVEYOR0
      -DNNG_HAVE_RESPONDENT0
      -DNNG_HAVE_LANGINFO=1)
    list(APPEND CSRCS ${NNG_DIR}/src/compat/nanomsg/nn.c
         ${TESTDIR}/compat_testutil.c ${TESTDIR}/convey.c ${TESTDIR}/trantest.c)
  endif()

  nuttx_add_library(nng STATIC)
  target_sources(nng PRIVATE ${CSRCS})
  target_compile_options(nng PRIVATE ${CFLAGS})
  target_include_directories(nng PRIVATE ${INCDIR})

  if(CONFIG_NETUTILS_NNG_TEST)
    # Set compile definitions for test files

    set_source_files_properties(
      ${TESTDIR}/bus.c
      PROPERTIES
        COMPILE_DEFINITIONS
        "bus=test_bus;bus1=test_bus1;bus2=test_bus2;bus3=test_bus3;rtimeo=test_bus_rtimeo"
    )

    set_source_files_properties(
      ${TESTDIR}/compat_block.c PROPERTIES COMPILE_DEFINITIONS
                                           "sc=cp_block_sc;sb=cp_block_sb")

    set_source_files_properties(
      ${TESTDIR}/compat_device.c
      PROPERTIES
        COMPILE_DEFINITIONS
        "deva=cp_device_deva;devb=cp_device_devb;devc=cp_device_devc;devd=cp_device_devd"
    )

    set_source_files_properties(${TESTDIR}/compat_poll.c
                                PROPERTIES COMPILE_DEFINITIONS sc=cp_poll_sc)

    set_source_files_properties(
      ${TESTDIR}/compat_reqttl.c
      PROPERTIES COMPILE_DEFINITIONS "dev0=cp_reqttl_dev0;dev1=cp_reqttl_dev1")

    set_source_files_properties(
      ${TESTDIR}/compat_surveyttl.c
      PROPERTIES COMPILE_DEFINITIONS
                 "dev0=cp_surveyttl_dev0;dev1=cp_surveyttl_dev1")

    set_source_files_properties(
      ${TESTDIR}/ipcsupp.c
      PROPERTIES
        COMPILE_DEFINITIONS
        "d=ipcsupp_dialer;l=ipcsupp_listener;daio=ipcsupp_aio;laio=ipcsupp_laio;maio=ipcsupp_maio;c1=ipcsupp_c1;c2=ipcsupp_c2;aio1=ipcsupp_aio1;aio2=ipcsupp_aio2;sa2=ipcsupp_sa2"
    )

    set_source_files_properties(
      ${TESTDIR}/ipcwinsec.c
      PROPERTIES COMPILE_DEFINITIONS
                 "s=ipcwinsec_s;l=ipcwinsec_l;x=ipcwinsec_x")

    set_source_files_properties(
      ${TESTDIR}/nonblock.c
      PROPERTIES COMPILE_DEFINITIONS
                 "addr=nonblock_addr;reqs=nonblock_reqs;rep=nonblock_rep")

    set_source_files_properties(
      ${TESTDIR}/pipe.c
      PROPERTIES COMPILE_DEFINITIONS
                 "addr=pipe_addr;cnt=pipe_cnt;push=pipe_push;pull=pipe_pull")

    set_source_files_properties(
      ${TESTDIR}/pollfd.c PROPERTIES COMPILE_DEFINITIONS
                                     "s1=pollfd_s1;s2=pollfd_s2;s3=pollfd_s3")

    set_source_files_properties(
      ${TESTDIR}/reqctx.c
      PROPERTIES
        COMPILE_DEFINITIONS
        "ctxs=reqctx_ctxs;saios=reqctx_saios;raios=reqctx_raios;recd=reqctx_recd;sent=reqctx_sent;req=reqctx_req;ctx=reqctx_ctx;aio=reqctx_aio"
    )

    set_source_files_properties(
      ${TESTDIR}/reqstress.c
      PROPERTIES
        COMPILE_DEFINITIONS
        "tcp4_template=reqstress_tcp4tmp;tcp6_template=reqstress_tcp6tmp;inproc_template=reqstress_inproctmp;ipc_template=reqstress_ipctemp;ws_template=reqstress_wstmp;templates=reqstress_templates;cases=reqstress_cases;ncases=reqstress_ncases;curcase=reqstress_curcase"
    )

    set_source_files_properties(
      ${TESTDIR}/stats.c PROPERTIES COMPILE_DEFINITIONS
                                    "s1=stats_s1;s2=stats_s2")

    set_source_files_properties(
      ${TESTDIR}/sync.c PROPERTIES COMPILE_DEFINITIONS
                                   "thr=sync_thr;mx=sync_mx")

    set_source_files_properties(${TESTDIR}/tcp6.c PROPERTIES COMPILE_DEFINITIONS
                                                             "s1=tcp6_s1")

    set_source_files_properties(
      ${TESTDIR}/tcpsupp.c
      PROPERTIES
        COMPILE_DEFINITIONS
        "d=tcpsupp_dialer;l=tcpsupp_listener;daio=tcpsupp_daio;laio=tcpsupp_laio;maio=tcpsupp_maio;c1=tcpsupp_c1;c2=tcpsupp_c2;aio1=tcpsupp_aio1;aio2=tcpsupp_aio2"
    )

    set_source_files_properties(
      ${TESTDIR}/udp.c PROPERTIES COMPILE_DEFINITIONS "u1=udp_u1;u2=udp_u2")

    set_source_files_properties(
      ${TESTDIR}/zt.c PROPERTIES COMPILE_DEFINITIONS "s=zt_s;s1=zt_s1;s2=zt_s2")

    macro(add_nng_test_app NNG_NAME NNG_MAIN)
      nuttx_add_application(
        NAME
        ${NNG_NAME}
        SRCS
        ${TESTDIR}/${NNG_MAIN}
        STACKSIZE
        ${CONFIG_NETUTILS_NNG_TEST_STACKSIZE}
        PRIORITY
        ${CONFIG_NETUTILS_NNG_TEST_PRIORITY}
        INCLUDE_DIRECTORIES
        ${NNG_DIR}/src
        COMPILE_FLAGS
        ${CFLAGS}
        DEPENDS
        nng)
    endmacro()

    add_nng_test_app(nng_test_bus bus.c)
    add_nng_test_app(nng_test_compat_block compat_block.c)
    add_nng_test_app(nng_test_compat_bug777 compat_bug777.c)
    add_nng_test_app(nng_test_compat_bus compat_bus.c)
    add_nng_test_app(nng_test_compat_cmsg compat_cmsg.c)
    add_nng_test_app(nng_test_compat_device compat_device.c)
    add_nng_test_app(nng_test_compat_iovec compat_iovec.c)
    add_nng_test_app(nng_test_compat_msg compat_msg.c)
    add_nng_test_app(nng_test_compat_options compat_options.c)
    add_nng_test_app(nng_test_compat_pair compat_pair.c)
    add_nng_test_app(nng_test_compat_pipeline compat_pipeline.c)
    add_nng_test_app(nng_test_compat_poll compat_poll.c)
    add_nng_test_app(nng_test_compat_reqrep compat_reqrep.c)
    add_nng_test_app(nng_test_compat_reqttl compat_reqttl.c)
    add_nng_test_app(nng_test_compat_shutdown compat_shutdown.c)
    add_nng_test_app(nng_test_compat_survey compat_survey.c)
    add_nng_test_app(nng_test_compat_surveyttl compat_surveyttl.c)
    add_nng_test_app(nng_test_device device.c)
    add_nng_test_app(nng_test_files files.c)
    add_nng_test_app(nng_test_httpclient httpclient.c)
    add_nng_test_app(nng_test_httpserver httpserver.c)
    add_nng_test_app(nng_test_inproc inproc.c)
    add_nng_test_app(nng_test_ipc ipc.c)
    add_nng_test_app(nng_test_ipcsupp ipcsupp.c)
    add_nng_test_app(nng_test_ipcwinsec ipcwinsec.c)
    add_nng_test_app(nng_test_multistress multistress.c)
    add_nng_test_app(nng_test_nonblock nonblock.c)
    add_nng_test_app(nng_test_pipe pipe.c)
    add_nng_test_app(nng_test_pollfd pollfd.c)
    add_nng_test_app(nng_test_reqctx reqctx.c)
    add_nng_test_app(nng_test_reqstress reqstress.c)
    add_nng_test_app(nng_test_scalability scalability.c)
    add_nng_test_app(nng_test_synch synch.c)
    add_nng_test_app(nng_test_tcp tcp.c)
    add_nng_test_app(nng_test_tcp6 tcp6.c)
    add_nng_test_app(nng_test_tcpsupp tcpsupp.c)
    add_nng_test_app(nng_test_tls tls.c)
    add_nng_test_app(nng_test_udp udp.c)
    add_nng_test_app(nng_test_ws ws.c)
    add_nng_test_app(nng_test_wss wss.c)
    add_nng_test_app(nng_test_zt zt.c)

  endif()
endif()
