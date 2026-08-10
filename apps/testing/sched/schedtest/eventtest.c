/****************************************************************************
 * apps/testing/sched/schedtest/eventtest.c
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

#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <nuttx/event.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define EVENT_MASK_1 0x01
#define EVENT_MASK_2 0x02
#define EVENT_MASK_4 0x04

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* Try to wait for the event1 */

FAR static void *thread_nxevent_trywait(FAR void *arg)
{
  FAR nxevent_t *event = arg;
  nxevent_mask_t result;

  result = nxevent_trywait(event, EVENT_MASK_1, 0);
  assert_int_equal(result, EVENT_MASK_1);

  return NULL;
}

/* Reset the event1 to event2 */

FAR static void *thread_nxevent_reset(FAR void *arg)
{
  FAR nxevent_t *event = arg;
  int ret;

  ret = nxevent_reset(event, EVENT_MASK_2);
  assert_int_equal(ret, 0);

  return NULL;
}

/* Wait for the event2 */

FAR static void *thread_nxevent_wait(FAR void *arg)
{
  FAR nxevent_t *event = arg;
  nxevent_mask_t result;

  result = nxevent_wait(event, EVENT_MASK_2, 0);
  assert_int_equal(result, EVENT_MASK_2);

  return NULL;
}

/* Post event4 */

FAR static void *thread_nxevent_post(FAR void *arg)
{
  FAR nxevent_t *event = arg;
  int ret;

  ret = nxevent_post(event, EVENT_MASK_4, NXEVENT_POST_SET);
  assert_int_equal(ret, 0);

  return NULL;
}

/* Wait for the event4 with timeout */

FAR static void *thread_nxevent_tickwait(FAR void *arg)
{
  FAR nxevent_t *event = arg;
  nxevent_mask_t result;

  result = nxevent_tickwait(event, EVENT_MASK_4, 0, 10);
  assert_int_equal(result, EVENT_MASK_4);

  return NULL;
}

static void test_trywait(FAR void **state)
{
  FAR nxevent_t *event = *state;
  pthread_t tid;

  assert_int_equal(pthread_create(&tid, NULL, thread_nxevent_trywait,
                   event), 0);

  assert_int_equal(pthread_join(tid, NULL), 0);
}

static void test_reset_and_wait(FAR void **state)
{
  FAR nxevent_t *event = *state;
  pthread_t r_tid;
  pthread_t w_tid;

  assert_int_equal(pthread_create(&r_tid, NULL, thread_nxevent_reset,
                   event), 0);
  assert_int_equal(pthread_create(&w_tid, NULL, thread_nxevent_wait,
                   event), 0);

  assert_int_equal(pthread_join(r_tid, NULL), 0);
  assert_int_equal(pthread_join(w_tid, NULL), 0);
}

static void test_post_and_tickwait(FAR void **state)
{
  FAR nxevent_t *event = *state;
  pthread_t p_tid;
  pthread_t t_tid;

  assert_int_equal(pthread_create(&p_tid, NULL, thread_nxevent_post,
                   event), 0);
  assert_int_equal(pthread_create(&t_tid, NULL, thread_nxevent_tickwait,
                   event), 0);

  assert_int_equal(pthread_join(p_tid, NULL), 0);
  assert_int_equal(pthread_join(t_tid, NULL), 0);
}

/****************************************************************************
 * Setup and Teardown Functions
 ****************************************************************************/

static int setup_nxevent_init(FAR void **state)
{
  FAR nxevent_t *event = (nxevent_t *)malloc(sizeof(nxevent_t));
  assert_non_null(event);
  *state = event;

  /* Initialize the event1 object */

  nxevent_init(event, EVENT_MASK_1);
  assert_int_equal(event->events, EVENT_MASK_1);

  return 0;
}

static int teardown_nxevent_destroy(FAR void **state)
{
  FAR nxevent_t *event = *state;
  int ret;

  /* Destroy the event object */

  ret = nxevent_destroy(event);
  assert_int_equal(ret, 0);

  assert_non_null(event);
  free(event);
  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  nxevent_t event;
  FAR void *state = &event;

  cmocka_set_message_output(CM_OUTPUT_STDOUT);
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test_prestate_setup_teardown(test_trywait,
                                               setup_nxevent_init,
                                               teardown_nxevent_destroy,
                                               &state),
      cmocka_unit_test_prestate_setup_teardown(test_reset_and_wait,
                                               setup_nxevent_init,
                                               teardown_nxevent_destroy,
                                               &state),
      cmocka_unit_test_prestate_setup_teardown(test_post_and_tickwait,
                                               setup_nxevent_init,
                                               teardown_nxevent_destroy,
                                               &state),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
