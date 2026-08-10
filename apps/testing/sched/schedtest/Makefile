############################################################################
# apps/testing/sched/schedtest/Makefile
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

PRIORITY  = $(CONFIG_TESTING_SCHEDTEST_PRIORITY)
STACKSIZE = $(CONFIG_TESTING_SCHEDTEST_STACKSIZE)
MODULE    = $(CONFIG_TESTING_SCHEDTEST)

CFLAGS  += -I$(TOPDIR)/sched
MAINSRC += rwsem.c signaltest.c

ifneq ($(CONFIG_SCHED_EVENTS),)
MAINSRC += eventtest.c
endif

ifeq ($(CONFIG_SMP),y)
MAINSRC += setaffinity_test.c
MAINSRC += getaffinity_test.c
MAINSRC += cpucount_test.c
ifeq ($(CONFIG_SCHED_CRITMONITOR),y)
ifneq ($(CONFIG_SCHED_CRITMONITOR_MAXTIME_BUSYWAIT),-1)
MAINSRC += busywait_test.c
endif
endif
endif

ifeq ($(CONFIG_SCHED_BACKTRACE),y)
MAINSRC += sched_backtrace_test.c
MAINSRC += sched_dumpstack_test.c
endif

ifneq ($(CONFIG_TLS_TASK_NELEM),0)
ifneq ($(CONFIG_TLS_NCLEANUP),0)
MAINSRC += tls_test.c
endif
endif

PROGNAME := $(notdir $(patsubst %.c,%, $(MAINSRC)))

include $(APPDIR)/Application.mk
