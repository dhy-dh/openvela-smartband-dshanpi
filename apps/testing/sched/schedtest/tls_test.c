/****************************************************************************
 * apps/testing/sched/schedtest/tls_test.c
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

#include <nuttx/config.h>
#include <nuttx/tls_task.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <cmocka.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void test_tls_task(FAR void **state)
{
  int tlsindex;
  uintptr_t test_value;
  int ret;

  /* Assign a TLS index */

  tlsindex = task_tls_alloc(NULL);
  assert_true(tlsindex >= 0);

  /* Setting TLS Values */

  test_value = 555;
  ret = task_tls_set_value(tlsindex, test_value);
  assert_int_equal(ret, 0);

  /* Get and verify TLS values */

  uintptr_t value = task_tls_get_value(tlsindex);
  assert_int_equal(value, test_value);

  /* Call task_tls_destruct to trigger the destructor */

  task_tls_destruct();
}

/**
 * Cleanup function for testing.
 */

void mock_cleanup_function(void *arg)
{
  int *value = (int *)arg;
  printf("Cleanup: %d\n", *value);
  free(value);
}

static void test_tls_cleanup(void **state)
{
  struct tls_info_s tls;

  /* Simulate the data to be cleaned. */

  int *data = malloc(sizeof(int));
  *data = 666;

  tls_cleanup_push(&tls, mock_cleanup_function, data);

  /* Check if the data has been added correctly to the stack. */

  assert_int_equal(tls.tl_tos, 1);

  /* Pop and perform cleanup. */

  tls_cleanup_pop(&tls, 1);

  /* Check if the stack is empty. */

  assert_int_equal(tls.tl_tos, 0);
}

static void test_tls_cleanup_popall(void **state)
{
  struct tls_info_s tls;

  int *data1 = malloc(sizeof(int));
  *data1 = 777;
  int *data2 = malloc(sizeof(int));
  *data2 = 888;

  tls_cleanup_push(&tls, mock_cleanup_function, data1);
  tls_cleanup_push(&tls, mock_cleanup_function, data2);

  assert_int_equal(tls.tl_tos, 2);

  /* Pop and perform any cleanup. */

  tls_cleanup_popall(&tls);

  /* Check if the stack is empty. */

  assert_int_equal(tls.tl_tos, 0);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_tls_task),
      cmocka_unit_test(test_tls_cleanup),
      cmocka_unit_test(test_tls_cleanup_popall),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
