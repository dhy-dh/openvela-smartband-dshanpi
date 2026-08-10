############################################################################
# apps/netutils/paho_mqtt/Makefile
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

SRCDIR = $(APPDIR)/netutils/paho_mqtt/paho_mqtt/src

ifeq ($(CONFIG_LIB_MQTT5), y)

ifeq ($(CONFIG_OPENSSL_MBEDTLS_WRAPPER), y)
SKIP  = $(SRCDIR)/SHA1.c
ifeq ($(CONFIG_LIB_MQTT5_TLS), y)
CFLAGS += -DOPENSSL
CFLAGS += -DSSL_OP_NO_TLSv1_2
CFLAGS += -DLIBRESSL_VERSION_NUMBER
CFLAGS += -DOPENSSL_VERSION_NUMBER=$(CONFIG_LIB_MQTT5_OPENSSL_VERSION_NUMBER)
endif
endif

ifeq ($(CONFIG_LIB_MQTT5_STACKTRACE),)
CFLAGS += -DNOSTACKTRACE
endif

SKIP += $(SRCDIR)/MQTTClient.c
SKIP += $(SRCDIR)/MQTTVersion.c

CSRCS = $(filter-out $(SKIP), $(wildcard $(SRCDIR)/*.c))

ifeq ($(CONFIG_UTILS_MQTT5),y)
CSRCS += $(SRCDIR)/samples/pubsub_opts.c
endif

VARS  = BUILD_TIMESTAMP PROJECT_VERSION PROJECT_VERSION_MAJOR
VARS += PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH

MQTT5_VERSION = $(SRCDIR)/VersionInfo.h

SED_COMMANDS = $(foreach var,$(VARS),-e 's/@$(var)@/$($(var))/g')

$(MQTT5_VERSION): $(SRCDIR)/VersionInfo.h.in
	sed $(SED_COMMANDS) $< > $@

context:: $(MQTT5_VERSION)

distclean::
	$(call DELFILE, $(MQTT5_VERSION))

ifeq ($(CONFIG_UTILS_MQTT5), y)
CSRCS += $(SRCDIR)/samples/pubsub_opts.c

PROGNAME  = mqtt_pub mqtt_sub
MAINSRC   = $(SRCDIR)/samples/paho_c_pub.c $(SRCDIR)/samples/paho_c_sub.c
PRIORITY  = $(CONFIG_UTILS_MQTT5_PRIORITY)
STACKSIZE = $(CONFIG_UTILS_MQTT5_STACKSIZE)
MODULE = $(CONFIG_UTILS_MQTT5)
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -DmessageArrived=mqtt_pub_messageArrived
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -DonDisconnect=mqtt_pub_onDisconnect
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -DonConnectFailure5=mqtt_pub_onConnectFailure5
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -DonConnectFailure=mqtt_pub_onConnectFailure
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -DonConnect5=mqtt_pub_onConnect5
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -DonConnect=mqtt_pub_onConnect
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -Dmysleep=mqtt_pub_mysleep -DtoStop=mqtt_pub_toStop
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -Dopts=mqtt_pub_opts -Dmyconnect=mqtt_pub_myconnect
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -Dcfinish=mqtt_pub_cfinish
$(SRCDIR)/samples/paho_c_pub.c_CFLAGS += -Dtrace_callback=mqtt_pub_trace_callback
endif

endif

include $(APPDIR)/Application.mk
