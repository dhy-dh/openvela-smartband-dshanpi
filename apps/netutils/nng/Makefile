############################################################################
# apps/netutils/nng/Makefile
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

NNG_VERSION := $(patsubst "%",%,$(CONFIG_NETUTILS_NNG_VERSION))
NNG_TARBALL  = v$(NNG_VERSION).tar.gz
NNG_UNPACK   = nng
NNG_SRCDIR   = $(NNG_UNPACK)$(DELIM)src

CFLAGS += -I$(NNG_UNPACK)/src
CFLAGS += -Wno-unused-value -Wno-shadow

CFLAGS += -DNNG_PLATFORM_POSIX
CFLAGS += -DNNG_HAVE_GETRANDOM
CFLAGS += -DNNG_TRANSPORT_INPROC
CFLAGS += -DNNG_TRANSPORT_IPC
CFLAGS += -DNNG_TRANSPORT_TCP
CFLAGS += -DNNG_TRANSPORT_TLS
CFLAGS += -DNNG_TRANSPORT_WS
CFLAGS += -DNNG_TRANSPORT_WSS
CFLAGS += -DNNG_SETSTACKSIZE
CFLAGS += -DNNG_HAVE_PTHREAD_SETNAME_NP

ifeq ($(CONFIG_NETUTILS_NNG_TEST),y)
CFLAGS += -Wno-strict-prototypes

CFLAGS += -DNNG_HAVE_PAIR0
CFLAGS += -DNNG_HAVE_BUS0
CFLAGS += -DNNG_HAVE_PULL0
CFLAGS += -DNNG_HAVE_PUSH0
CFLAGS += -DNNG_HAVE_PUB0
CFLAGS += -DNNG_HAVE_SUB0
CFLAGS += -DNNG_HAVE_REQ0
CFLAGS += -DNNG_HAVE_REP0
CFLAGS += -DNNG_HAVE_SURVEYOR0
CFLAGS += -DNNG_HAVE_RESPONDENT0
CFLAGS += -DNNG_HAVE_LANGINFO=1
endif

ifneq ($(CONFIG_SMP_NCPUS),)
CFLAGS += -DNNG_MAX_TASKQ_THREADS=$(CONFIG_SMP_NCPUS)
else
CFLAGS += -DNNG_MAX_TASKQ_THREADS=2
endif

CFLAGS += -DNNG_USE_EVENTFD
ifeq ($(CONFIG_NETUTILS_NNG_HAVE_EPOLL),y)
CFLAGS += -DNNG_HAVE_EPOLL
CFLAGS += -DNNG_HAVE_EPOLL_CREATE1
endif

ifeq ($(CONFIG_NETUTILS_NNG_USE_MBEDTLS),y)
CFLAGS += -DNNG_TLS_ENGINE_INIT=nng_tls_engine_init_mbed
CFLAGS += -DNNG_TLS_ENGINE_FINI=nng_tls_engine_fini_mbed
CFLAGS += -DNNG_SUPP_TLS
endif

DEPPATH += --dep-path $(NNG_SRCDIR)
VPATH   += :$(NNG_SRCDIR)

CSRCS  = $(NNG_SRCDIR)/nng.c
CSRCS += $(NNG_SRCDIR)/nng_legacy.c
CSRCS += $(NNG_SRCDIR)/core/aio.c
CSRCS += $(NNG_SRCDIR)/core/device.c
CSRCS += $(NNG_SRCDIR)/core/dialer.c
CSRCS += $(NNG_SRCDIR)/core/file.c
CSRCS += $(NNG_SRCDIR)/core/idhash.c
CSRCS += $(NNG_SRCDIR)/core/init.c
CSRCS += $(NNG_SRCDIR)/core/list.c
CSRCS += $(NNG_SRCDIR)/core/listener.c
CSRCS += $(NNG_SRCDIR)/core/lmq.c
CSRCS += $(NNG_SRCDIR)/core/message.c
CSRCS += $(NNG_SRCDIR)/core/msgqueue.c
CSRCS += $(NNG_SRCDIR)/core/nng_global.c
CSRCS += $(NNG_SRCDIR)/core/options.c
CSRCS += $(NNG_SRCDIR)/core/pollable.c
CSRCS += $(NNG_SRCDIR)/core/panic.c
CSRCS += $(NNG_SRCDIR)/core/pipe.c
CSRCS += $(NNG_SRCDIR)/core/protocol.c
CSRCS += $(NNG_SRCDIR)/core/reap.c
CSRCS += $(NNG_SRCDIR)/core/socket.c
CSRCS += $(NNG_SRCDIR)/core/stats.c
CSRCS += $(NNG_SRCDIR)/core/stream.c
CSRCS += $(NNG_SRCDIR)/core/strs.c
CSRCS += $(NNG_SRCDIR)/core/taskq.c
CSRCS += $(NNG_SRCDIR)/core/tcp.c
CSRCS += $(NNG_SRCDIR)/core/thread.c
CSRCS += $(NNG_SRCDIR)/core/timer.c
CSRCS += $(NNG_SRCDIR)/core/url.c

CSRCS += $(NNG_SRCDIR)/platform/posix/posix_alloc.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_atomic.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_clock.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_debug.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_file.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_ipcconn.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_ipcdial.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_ipclisten.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_pipe.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_resolv_gai.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_sockaddr.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_tcpconn.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_tcpdial.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_tcplisten.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_thread.c
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_udp.c
ifeq ($(CONFIG_NETUTILS_NNG_HAVE_EPOLL),y)
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_pollq_epoll.c
else
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_pollq_poll.c
endif
CSRCS += $(NNG_SRCDIR)/platform/posix/posix_rand_getrandom.c

CSRCS += $(NNG_SRCDIR)/sp/transport.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/bus0/bus.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/pair0/pair.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/pair1/pair.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/pair1/pair1_poly.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/pipeline0/pull.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/pipeline0/push.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/pubsub0/pub.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/pubsub0/sub.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/pubsub0/xsub.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/reqrep0/rep.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/reqrep0/req.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/reqrep0/xrep.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/reqrep0/xreq.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/survey0/respond.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/survey0/survey.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/survey0/xrespond.c
CSRCS += $(NNG_SRCDIR)/sp/protocol/survey0/xsurvey.c
CSRCS += $(NNG_SRCDIR)/sp/transport/inproc/inproc.c
CSRCS += $(NNG_SRCDIR)/sp/transport/ipc/ipc.c
CSRCS += $(NNG_SRCDIR)/sp/transport/tcp/tcp.c
CSRCS += $(NNG_SRCDIR)/sp/transport/tls/tls.c
CSRCS += $(NNG_SRCDIR)/sp/transport/ws/websocket.c

CSRCS += $(NNG_SRCDIR)/supplemental/base64/base64.c
CSRCS += $(NNG_SRCDIR)/supplemental/http/http_chunk.c
CSRCS += $(NNG_SRCDIR)/supplemental/http/http_client.c
CSRCS += $(NNG_SRCDIR)/supplemental/http/http_conn.c
CSRCS += $(NNG_SRCDIR)/supplemental/http/http_msg.c
CSRCS += $(NNG_SRCDIR)/supplemental/http/http_public.c
CSRCS += $(NNG_SRCDIR)/supplemental/http/http_schemes.c
CSRCS += $(NNG_SRCDIR)/supplemental/http/http_server.c
CSRCS += $(NNG_SRCDIR)/supplemental/sha1/sha1.c
CSRCS += $(NNG_SRCDIR)/supplemental/tls/tls_common.c
ifeq ($(CONFIG_NETUTILS_NNG_USE_MBEDTLS),y)
CSRCS += $(NNG_SRCDIR)/supplemental/tls/mbedtls/tls.c
endif
CSRCS += $(NNG_SRCDIR)/supplemental/util/platform.c
CSRCS += $(NNG_SRCDIR)/supplemental/websocket/websocket.c

ifeq ($(CONFIG_NETUTILS_NNG_TEST),y)
PRIORITY  = $(CONFIG_NETUTILS_NNG_TEST_PRIORITY)
STACKSIZE = $(CONFIG_NETUTILS_NNG_TEST_STACKSIZE)
CSRCS += nng/src/compat/nanomsg/nn.c
CSRCS += nng/tests/compat_testutil.c
CSRCS += nng/tests/convey.c
CSRCS += nng/tests/trantest.c

nng/tests/bus.c_CFLAGS += -Dbus=test_bus
nng/tests/bus.c_CFLAGS += -Dbus1=test_bus1
nng/tests/bus.c_CFLAGS += -Dbus2=test_bus2
nng/tests/bus.c_CFLAGS += -Dbus3=test_bus3
nng/tests/bus.c_CFLAGS += -Drtimeo=test_bus_rtimeo
nng/tests/compat_block.c_CFLAGS += -Dsc=cp_block_sc
nng/tests/compat_block.c_CFLAGS += -Dsb=cp_block_sb
nng/tests/compat_device.c_CFLAGS += -Ddeva=cp_device_deva
nng/tests/compat_device.c_CFLAGS += -Ddevb=cp_device_devb
nng/tests/compat_device.c_CFLAGS += -Ddevc=cp_device_devc
nng/tests/compat_device.c_CFLAGS += -Ddevd=cp_device_devd
nng/tests/compat_poll.c_CFLAGS += -Dsc=cp_poll_sc
nng/tests/compat_reqttl.c_CFLAGS += -Ddev0=cp_reqttl_dev0
nng/tests/compat_reqttl.c_CFLAGS += -Ddev1=cp_reqttl_dev1
nng/tests/compat_surveyttl.c_CFLAGS += -Ddev0=cp_surveyttl_dev0
nng/tests/compat_surveyttl.c_CFLAGS += -Ddev1=cp_surveyttl_dev1
nng/tests/ipcsupp.c_CFLAGS += -Dd=ipcsupp_dialer
nng/tests/ipcsupp.c_CFLAGS += -Dl=ipcsupp_listener
nng/tests/ipcsupp.c_CFLAGS += -Ddaio=ipcsupp_aio
nng/tests/ipcsupp.c_CFLAGS += -Dlaio=ipcsupp_laio
nng/tests/ipcsupp.c_CFLAGS += -Dmaio=ipcsupp_maio
nng/tests/ipcsupp.c_CFLAGS += -Dc1=ipcsupp_c1
nng/tests/ipcsupp.c_CFLAGS += -Dc2=ipcsupp_c2
nng/tests/ipcsupp.c_CFLAGS += -Daio1=ipcsupp_aio1
nng/tests/ipcsupp.c_CFLAGS += -Daio2=ipcsupp_aio2
nng/tests/ipcsupp.c_CFLAGS += -Dsa2=ipcsupp_sa2
nng/tests/ipcwinsec.c_CFLAGS += -Ds=ipcwinsec_s
nng/tests/ipcwinsec.c_CFLAGS += -Dl=ipcwinsec_l
nng/tests/ipcwinsec.c_CFLAGS += -Dx=ipcwinsec_x
nng/tests/nonblock.c_CFLAGS += -Daddr=nonblock_addr
nng/tests/nonblock.c_CFLAGS += -Dreqs=nonblock_reqs
nng/tests/nonblock.c_CFLAGS += -Drep=nonblock_rep
nng/tests/pipe.c_CFLAGS += -Daddr=pipe_addr
nng/tests/pipe.c_CFLAGS += -Dcnt=pipe_cnt
nng/tests/pipe.c_CFLAGS += -Dpush=pipe_push
nng/tests/pipe.c_CFLAGS += -Dpull=pipe_pull
nng/tests/pollfd.c_CFLAGS += -Ds1=pollfd_s1
nng/tests/pollfd.c_CFLAGS += -Ds2=pollfd_s2
nng/tests/pollfd.c_CFLAGS += -Ds3=pollfd_s3
nng/tests/reqctx.c_CFLAGS += -Dctxs=reqctx_ctxs
nng/tests/reqctx.c_CFLAGS += -Dsaios=reqctx_saios
nng/tests/reqctx.c_CFLAGS += -Draios=reqctx_raios
nng/tests/reqctx.c_CFLAGS += -Drecd=reqctx_recd
nng/tests/reqctx.c_CFLAGS += -Dsent=reqctx_sent
nng/tests/reqctx.c_CFLAGS += -Dreq=reqctx_req
nng/tests/reqctx.c_CFLAGS += -Dctx=reqctx_ctx
nng/tests/reqctx.c_CFLAGS += -Daio=reqctx_aio
nng/tests/reqstress.c_CFLAGS += -Dtcp4_template=reqstress_tcp4tmp
nng/tests/reqstress.c_CFLAGS += -Dtcp6_template=reqstress_tcp6tmp
nng/tests/reqstress.c_CFLAGS += -Dinproc_template=reqstress_inproctmp
nng/tests/reqstress.c_CFLAGS += -Dipc_template=reqstress_ipctemp
nng/tests/reqstress.c_CFLAGS += -Dws_template=reqstress_wstmp
nng/tests/reqstress.c_CFLAGS += -Dtemplates=reqstress_templates
nng/tests/reqstress.c_CFLAGS += -Dcases=reqstress_cases
nng/tests/reqstress.c_CFLAGS += -Dncases=reqstress_ncases
nng/tests/reqstress.c_CFLAGS += -Dcurcase=reqstress_curcase
nng/tests/stats.c_CFLAGS += -Ds1=stats_s1
nng/tests/stats.c_CFLAGS += -Ds2=stats_s2
nng/tests/sync.c_CFLAGS += -Dthr=sync_thr
nng/tests/sync.c_CFLAGS += -Dmx=sync_mx
nng/tests/tcp6.c_CFLAGS += -Ds1=tcp6_s1
nng/tests/tcpsupp.c_CFLAGS += -Dd=tcpsupp_dialer
nng/tests/tcpsupp.c_CFLAGS += -Dl=tcpsupp_listener
nng/tests/tcpsupp.c_CFLAGS += -Ddaio=tcpsupp_daio
nng/tests/tcpsupp.c_CFLAGS += -Dlaio=tcpsupp_laio
nng/tests/tcpsupp.c_CFLAGS += -Dmaio=tcpsupp_maio
nng/tests/tcpsupp.c_CFLAGS += -Dc1=tcpsupp_c1
nng/tests/tcpsupp.c_CFLAGS += -Dc2=tcpsupp_c2
nng/tests/tcpsupp.c_CFLAGS += -Daio1=tcpsupp_aio1
nng/tests/tcpsupp.c_CFLAGS += -Daio2=tcpsupp_aio2
nng/tests/udp.c_CFLAGS += -Du1=udp_u1
nng/tests/udp.c_CFLAGS += -Du2=udp_u2
nng/tests/zt.c_CFLAGS += -Ds=zt_s
nng/tests/zt.c_CFLAGS += -Ds1=zt_s1
nng/tests/zt.c_CFLAGS += -Ds2=zt_s2

TESTS_NNG := compat_block compat_cmsg compat_msg compat_pipeline compat_reqttl \
compat_surveyttl compat_bug777 compat_device compat_options compat_poll \
compat_shutdown compat_bus compat_iovec compat_pair compat_reqrep compat_survey \
bus files ipc nonblock tcp udp httpclient ipcsupp reqctx tcpsupp ws \
httpserver ipcwinsec pipe reqstress synch tls wss device inproc \
multistress pollfd scalability tcp6 zt

$(foreach test, $(TESTS_NNG), $(eval PROGNAME += nng_test_$(test)) \
$(eval MAINSRC += nng/tests/$(test).c))
endif

$(NNG_TARBALL):
	$(Q) echo "Downloading nng-$(NNG_VERSION)"
	$(Q) curl -O -L https://github.com/nanomsg/nng/archive/$(NNG_TARBALL)

$(NNG_UNPACK): $(NNG_TARBALL)
	$(Q) tar zxf $(NNG_TARBALL)
	$(Q) mv nng-$(NNG_VERSION) $(NNG_UNPACK)
	$(Q) echo "Patching $(NNG_UNPACK)"
	$(Q) patch -p0 < nng_porting_for_nuttx.patch
	$(Q) touch $(NNG_UNPACK)

# Download and unpack tarball if no git repo found
ifeq ($(wildcard $(NNG_UNPACK)/.git),)
context:: $(NNG_UNPACK)

distclean::
	$(call DELFILE, $(NNG_TARBALL))
	$(call DELDIR, $(NNG_UNPACK))
endif

include $(APPDIR)/Application.mk
