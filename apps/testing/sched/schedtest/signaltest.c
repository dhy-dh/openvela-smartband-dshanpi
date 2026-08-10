/****************************************************************************
 * apps/testing/sched/schedtest/signaltest.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <nuttx/signal.h>
#include <sys/wait.h>

#include "sched/sched.h"
#include "signal/signal.h"

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

struct test_data_t
{
  siginfo_t siginfo;
  struct task_group_s group;
  struct tcb_s tcb;
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void my_signal_handler(int signo)
{
  assert_int_equal(signo, SIGUSR1);
}

static void test_nxsig_action(FAR void **state)
{
  FAR struct test_data_t *data;
  struct sigaction new_action;
  struct sigaction old_action;
  sigset_t expected_mask;
  int ret;

  data = (FAR struct test_data_t *)*state;
  memset(&data->group, 0, sizeof(data->group));

  sq_init(&data->group.tg_sigactionq);
  data->tcb.group = &data->group;

  new_action.sa_u._sa_handler = my_signal_handler;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;

  ret = nxsig_action(SIGUSR1, &new_action, &old_action, false);
  raise(SIGUSR1);
  assert_int_equal(ret, 0);
  assert_ptr_equal(old_action.sa_handler, SIG_IGN);

  nxsig_cleanup(&data->tcb);

  sigfillset(&expected_mask);
  assert_memory_equal(&data->tcb.sigprocmask, &expected_mask,
                      sizeof(sigset_t));
  sigemptyset(&expected_mask);
  assert_memory_equal(&data->tcb.sigwaitmask, &expected_mask,
                      sizeof(sigset_t));

  nxsig_release(&data->group);
}

#ifdef CONFIG_SIG_DEFAULT
static void test_nxsig_default_initialize(FAR void **state)
{
  FAR struct test_data_t *data;
  data = (FAR struct test_data_t *)*state;

  data->tcb.group = &data->group;

  int result = nxsig_default_initialize(&data->tcb);
  assert_int_equal(result, OK);
}
#endif

static void test_nxsig_find_action(FAR void **state)
{
  FAR struct test_data_t *data;
  sigactq_t sigaction1;
  sigactq_t sigaction2;
  sigactq_t sigaction3;
  FAR sigactq_t *result1 = NULL;
  FAR sigactq_t *result2 = NULL;

  data = (FAR struct test_data_t *)*state;
  sigaction1.signo = 1;
  sigaction2.signo = 2;
  sigaction3.signo = 3;
  sigaction1.flink = &sigaction2;
  sigaction2.flink = &sigaction3;
  sigaction3.flink = NULL;

  data->group.tg_sigactionq.head = (sq_entry_t *)&sigaction1;

  result1 = nxsig_find_action(&data->group, 2);
  assert_non_null(result1);
  assert_int_equal(result1->signo, 2);

  result2 = nxsig_find_action(&data->group, 4);
  assert_null(result2);
}

static void *thread_nxsig_kill(FAR void *arg)
{
  while (1)
    {
      sleep(1);
    }

  return NULL;
}

static void test_nxsig_kill(FAR void **state)
{
  UNUSED(state);
  pthread_t thread;

  int ret = pthread_create(&thread, NULL, thread_nxsig_kill, NULL);
  assert_true(ret == 0);

  sleep(1);
  assert_int_equal(nxsig_kill(-1, 1), -ENOSYS);
  assert_int_equal(nxsig_kill(thread, MAX_SIGNO + 1), -EINVAL);
  assert_int_equal(nxsig_kill(thread, SIGUSR1), 0);

  pthread_cancel(thread);
  pthread_join(thread, NULL);
}

static void test_nxsig_lowest(FAR void **state)
{
  UNUSED(state);
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, 5);
  sigaddset(&set, 4);
  sigaddset(&set, 3);

  assert_int_equal(nxsig_lowest(&set), 3);
}

static void test_nxsig_nanosleep(FAR void **state)
{
  UNUSED(state);
  struct timespec rqtp;
  struct timespec rmtp;
  struct timespec start;
  struct timespec end;
  double elapsed_time;
  int ret;

  rqtp.tv_sec = 0;
  rqtp.tv_nsec = 1000000000;
  assert_int_equal(nxsig_nanosleep(&rqtp, &rmtp), -EINVAL);

  rqtp.tv_sec = 0;
  rqtp.tv_nsec = 500000000;
  clock_gettime(CLOCK_MONOTONIC, &start);
  ret = nxsig_nanosleep(&rqtp, &rmtp);
  clock_gettime(CLOCK_MONOTONIC, &end);
  assert_int_equal(ret, 0);

  elapsed_time = (end.tv_sec - start.tv_sec) +
                 (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  assert_true(elapsed_time >= 0.5 && elapsed_time < 0.6);
}

#ifdef CONFIG_SIG_EVTHREAD
static void nxsig_notification_work(union sigval sv)
{
  int value = (int)(intptr_t)sv.sival_ptr;
  assert_int_equal(value, 42);
}
#endif

static void *thread_nxsig_notification(FAR void *arg)
{
  while (1)
    {
      sleep(1);
    }

  return NULL;
}

static void test_nxsig_notification(FAR void **state)
{
  UNUSED(state);
  struct sigevent event;
#ifdef CONFIG_SIG_EVTHREAD
  struct sigwork_s work;
#endif
  int notify_value = 42;
  pthread_t thread;
  int ret;

  ret = pthread_create(&thread, NULL, thread_nxsig_notification, NULL);
  assert_true(ret == 0);

  sleep(1);
  event.sigev_notify = SIGEV_SIGNAL;
  event.sigev_signo = SIGUSR1;
  event.sigev_value.sival_ptr = (void *)(intptr_t)notify_value;
  ret = nxsig_notification(thread, &event, SI_USER, NULL);
  assert_true(ret == 0);

#ifdef CONFIG_SIG_EVTHREAD
  event.sigev_notify = SIGEV_THREAD;
  event.sigev_value.sival_ptr = (void *)(intptr_t)notify_value;
  event.sigev_notify_function = nxsig_notification_work;
  memset(&work, 0, sizeof(work));
  ret = nxsig_notification(thread, &event, SI_USER, &work);
  assert_true(ret == 0);
  nxsig_cancel_notification(&work);
#endif

  event.sigev_notify = SIGEV_NONE;
  assert_int_equal(nxsig_notification(0, &event, 0, NULL), 0);

  pthread_cancel(thread);
  pthread_join(thread, NULL);
}

static void test_nxsig_pendingset(FAR void **state)
{
  FAR struct test_data_t *data;
  sigpendq_t mock_sigpend1;
  sigpendq_t mock_sigpend2;

  data = (FAR struct test_data_t *)*state;

  data->tcb.group = &data->group;
  data->group.tg_sigpendingq.head = (sq_entry_t *)&mock_sigpend1;

  mock_sigpend1.info.si_signo = SIGUSR1;
  mock_sigpend1.flink = &mock_sigpend2;

  mock_sigpend2.info.si_signo = SIGUSR2;
  mock_sigpend2.flink = NULL;

  sigset_t result = nxsig_pendingset(&data->tcb);

  assert_true(sigismember(&result, SIGUSR1));
  assert_true(sigismember(&result, SIGUSR2));
}

static void test_nxsig_procmask(FAR void **state)
{
  FAR struct test_data_t *data;
  sigset_t initial_mask;
  sigset_t new_mask;
  sigset_t old_mask;
  FAR struct tcb_s *current_task = this_task();

  data = (FAR struct test_data_t *)*state;

  sigemptyset(&initial_mask);
  sigemptyset(&new_mask);

  sigaddset(&initial_mask, SIGUSR1);
  sigaddset(&new_mask, SIGUSR2);

  current_task->sigprocmask = initial_mask;

  int ret = nxsig_procmask(SIG_BLOCK, &new_mask, &old_mask);

  memcpy(&(data->tcb), current_task, sizeof(struct tcb_s));

  assert_int_equal(ret, OK);
  assert_true(sigismember(&old_mask, SIGUSR1));
  assert_true(sigismember(&(data->tcb.sigprocmask), SIGUSR1));
  assert_true(sigismember(&(data->tcb.sigprocmask), SIGUSR2));
}

FAR static void *thread_nxsig_queue(FAR void *arg)
{
  struct siginfo signo;
  sigset_t set;
  int ret;

  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);

  ret = nxsig_timedwait(&set, &signo, NULL);
  assert_int_equal(ret, SIGUSR1);

  return NULL;
}

static void test_nxsig_queue(FAR void **state)
{
  UNUSED(state);
  union sigval valid_value;
  pthread_t thread;
  int valid_signo = SIGUSR1;
  int invalid_signo = -1;
  int ret;
  valid_value.sival_int = 42;

  ret = pthread_create(&thread, NULL, thread_nxsig_queue, NULL);
  assert_true(ret == 0);

  sleep(1);
  ret = nxsig_queue(thread, invalid_signo, valid_value);
  assert_int_equal(ret, -EINVAL);

  ret = nxsig_queue(thread, valid_signo, valid_value);
  assert_int_equal(ret, 0);
}

static void test_nxsig_sleep(FAR void **state)
{
  UNUSED(state);
  struct timespec start;
  struct timespec end;
  double elapsed_time;
  unsigned int seconds = 2;
  unsigned int ret;

  clock_gettime(CLOCK_MONOTONIC, &start);
  ret = nxsig_sleep(seconds);
  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed_time = (end.tv_sec - start.tv_sec) +
                 (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  assert_true(elapsed_time >= 2.0 && elapsed_time < 2.1);
  assert_int_equal(ret, 0);
}

FAR static void *thread_nxsig_tgkill(FAR void *arg)
{
  while (1)
    {
      pause();
    }

  return NULL;
}

static void test_nxsig_tgkill(void **state)
{
  UNUSED(state);
  pid_t pid;
  pthread_t thread;
  int signo = SIGUSR1;
  int ret;

  ret = pthread_create(&thread, NULL, thread_nxsig_tgkill, NULL);
  assert_true(ret == 0);

  sleep(1);
  pid = getpid();
  assert_int_equal(nxsig_tgkill(pid, thread, -1), -EINVAL);
  assert_int_equal(nxsig_tgkill(pid, thread, signo), 0);

  pthread_cancel(thread);
  pthread_join(thread, NULL);
}

static void test_nxsig_usleep(void **state)
{
  UNUSED(state);
  useconds_t usec = 1500000;
  struct timespec start;
  struct timespec end;
  double elapsed_time;
  int ret;

  clock_gettime(CLOCK_MONOTONIC, &start);
  ret = nxsig_usleep(usec);
  clock_gettime(CLOCK_MONOTONIC, &end);
  assert_int_equal(ret, 0);

  elapsed_time = (end.tv_sec - start.tv_sec) +
                 (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  assert_true(elapsed_time >= 1.5 && elapsed_time < 1.6);
}

static void test_nxsig_tcbdispatch(FAR void **state)
{
  FAR struct test_data_t *data;
  data = (FAR struct test_data_t *)*state;

  int ret = nxsig_tcbdispatch(&data->tcb, &data->siginfo);
  assert_int_equal(ret, OK);
}

static void *thread_nxsig_dispatch(FAR void *arg)
{
  FAR struct test_data_t *data = (struct test_data_t *)arg;

  data->siginfo.si_signo = SIGUSR1;
  data->tcb.pid = getpid();

  int ret = nxsig_dispatch(data->tcb.pid, &data->siginfo, false);
  assert_int_equal(ret, OK);

  return NULL;
}

static void test_nxsig_dispatch(FAR void **state)
{
  struct test_data_t *data = (struct test_data_t *)*state;
  pthread_t thread;

  int err = pthread_create(&thread, NULL, thread_nxsig_dispatch, data);
  assert_int_equal(err, 0);

  pthread_join(thread, NULL);
}

/****************************************************************************
 * Setup and Teardown Functions
 ****************************************************************************/

static int test_setup(FAR void **state)
{
  FAR struct test_data_t *data = malloc(sizeof(struct test_data_t));
  assert_non_null(data);

  memset(data, 0, sizeof(struct test_data_t));
  *state = data;

  return 0;
}

static int test_teardown(FAR void **state)
{
  assert_non_null(*state);
  free(*state);
  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  struct test_data_t *data = NULL;

  cmocka_set_message_output(CM_OUTPUT_STDOUT);
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(test_nxsig_kill),
      cmocka_unit_test(test_nxsig_lowest),
      cmocka_unit_test(test_nxsig_nanosleep),
      cmocka_unit_test(test_nxsig_queue),
      cmocka_unit_test(test_nxsig_sleep),
      cmocka_unit_test(test_nxsig_tgkill),
      cmocka_unit_test(test_nxsig_usleep),
      cmocka_unit_test(test_nxsig_notification),
#ifdef CONFIG_SIG_DEFAULT
      cmocka_unit_test_prestate_setup_teardown(test_nxsig_default_initialize,
                                               test_setup,
                                               test_teardown,
                                               data),
#endif
      cmocka_unit_test_prestate_setup_teardown(test_nxsig_find_action,
                                               test_setup,
                                               test_teardown,
                                               data),
      cmocka_unit_test_prestate_setup_teardown(test_nxsig_pendingset,
                                               test_setup,
                                               test_teardown,
                                               data),
      cmocka_unit_test_prestate_setup_teardown(test_nxsig_action,
                                               test_setup,
                                               test_teardown,
                                               data),
      cmocka_unit_test_prestate_setup_teardown(test_nxsig_procmask,
                                               test_setup,
                                               test_teardown,
                                               data),
      cmocka_unit_test_prestate_setup_teardown(test_nxsig_tcbdispatch,
                                               test_setup,
                                               test_teardown,
                                               data),
      cmocka_unit_test_prestate_setup_teardown(test_nxsig_dispatch,
                                               test_setup,
                                               test_teardown,
                                               data),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
