# ##############################################################################
# apps/testing/sched/schedtest/CMakeLists.txt
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for
# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

if(CONFIG_TESTING_SCHEDTEST)

  if(CONFIG_SCHED_EVENTS)
    nuttx_add_application(
      NAME
      eventtest
      PRIORITY
      ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
      MODULE
      ${CONFIG_TESTING_SCHEDTEST}
      SRCS
      eventtest.c)
  endif()

  nuttx_add_application(
    NAME
    rwsemtest
    PRIORITY
    ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
    STACKSIZE
    ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
    MODULE
    ${CONFIG_TESTING_SCHEDTEST}
    SRCS
    rwsem.c)

  if(CONFIG_SMP)
    nuttx_add_application(
      NAME
      setaffinity_test
      PRIORITY
      ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
      MODULE
      ${CONFIG_TESTING_SCHEDTEST}
      SRCS
      setaffinity_test.c)
    nuttx_add_application(
      NAME
      getaffinity_test
      PRIORITY
      ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
      MODULE
      ${CONFIG_TESTING_SCHEDTEST}
      SRCS
      getaffinity_test.c)
    nuttx_add_application(
      NAME
      cpucount_test
      PRIORITY
      ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
      MODULE
      ${CONFIG_TESTING_SCHEDTEST}
      SRCS
      cpucount_test.c)

    if(CONFIG_SCHED_CRITMONITOR)
      if(NOT CONFIG_SCHED_CRITMONITOR_MAXTIME_BUSYWAIT -1)
        nuttx_add_application(
          NAME
          busywait_test
          PRIORITY
          ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
          STACKSIZE
          ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
          MODULE
          ${CONFIG_TESTING_SCHEDTEST}
          INCLUDE_DIRECTORIES
          ${NUTTX_DIR}/sched
          SRCS
          busywait_test.c)
      endif()
    endif()
  endif()

  if(CONFIG_SCHED_BACKTRACE)
    nuttx_add_application(
      NAME
      sched_backtrace_test
      PRIORITY
      ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
      MODULE
      ${CONFIG_TESTING_SCHEDTEST}
      SRCS
      sched_backtrace_test.c)
    nuttx_add_application(
      NAME
      sched_dumpstack_test
      PRIORITY
      ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
      MODULE
      ${CONFIG_TESTING_SCHEDTEST}
      SRCS
      sched_dumpstack_test.c)
  endif()

  nuttx_add_application(
    NAME
    signaltest
    INCLUDE_DIRECTORIES
    ${NUTTX_DIR}/sched
    PRIORITY
    ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
    STACKSIZE
    ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
    MODULE
    ${CONFIG_TESTING_SCHEDTEST}
    SRCS
    signaltest.c)

  if(NOT CONFIG_TLS_TASK_NELEM EQUAL 0 AND NOT CONFIG_TLS_NCLEANUP EQUAL 0)
    nuttx_add_application(
      NAME
      tls_test
      PRIORITY
      ${CONFIG_TESTING_SCHEDTEST_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_SCHEDTEST_STACKSIZE}
      MODULE
      ${CONFIG_TESTING_SCHEDTEST}
      SRCS
      tls_test.c)
  endif()
endif()
