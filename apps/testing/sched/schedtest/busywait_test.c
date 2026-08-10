/****************************************************************************
 * apps/testing/sched/schedtest/busywait_test.c
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
#include <sched/sched.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <cmocka.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WAIT_MIN 1
#define WAIT_MAX 10000
#define MDELAY 1000

/****************************************************************************
 * Private Data
 ****************************************************************************/

static spinlock_t g_test_lock = SP_UNLOCKED;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

FAR static void *busy_loop(FAR void *arg)
{
  int cpu               = this_cpu();
  int wait_time         = *(FAR int *)arg;
  FAR struct tcb_s *tcb = current_task(cpu);
  irqstate_t flags;

  clock_t current = perf_gettime();
  printf("Thread: %d; CPU: %d; current_time = %"PRIu64"\n",
         tcb->pid, cpu, current);
  printf("tcb->busywait_max = %"PRIu64"; tcb->busywait_total = %"PRIu64"\n",
         tcb->busywait_max, tcb->busywait_total);

  /* Use up_mdelay to simulate synchronous operation between multiple cores */

  flags = spin_lock_irqsave(&g_test_lock);
  up_mdelay(wait_time);
  spin_unlock_irqrestore(&g_test_lock, flags);

  /* record the max busywait time */

  printf("Thread: %d; CPU: %d; tcb->busywait_start =%"PRIu64"; "
         "tcb->busywait_max = %"PRIu64"; tcb->busywait_total = %"PRIu64"; "
         "g_busywait_total[%d] = %"PRIu64"\n",
         tcb->pid, cpu, tcb->busywait_start, tcb->busywait_max,
         tcb->busywait_total, cpu, g_busywait_total[cpu]);

  assert_true(tcb->busywait_max <= tcb->busywait_total);
  assert_true(tcb->busywait_total <= g_busywait_total[cpu]);

  return NULL;
}

/**
 * Test the critical section.
 */

static void test_critical_section(void **state)
{
  pthread_attr_t attr;
  pthread_t      thread[2];
  int            i;
  int            ret;
  cpu_set_t      cpuset;
  int            wait_time;
  clock_t        last_allcpu;

  printf("\ntest_critical_section: Test start\n");
  for (wait_time = WAIT_MIN; wait_time <= WAIT_MAX; wait_time *= 10)
    {
      last_allcpu = g_busywait_total[0] + g_busywait_total[1];
      printf("\n-----------------wait_time = %d--------------\n", wait_time);

      /* Creating multiple threads */

      for (i = 0; i < 2; i++)
        {
          CPU_ZERO(&cpuset);
          CPU_SET(i, &cpuset);
          pthread_attr_init(&attr);
          pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
          ret = pthread_create(&thread[i], &attr, busy_loop, &wait_time);
          assert_int_equal(ret, 0);
        }

      for (i = 0; i < 2; i++)
        {
          pthread_join(thread[i], NULL);
        }

      assert_true(last_allcpu <= g_busywait_total[0] + g_busywait_total[1]);
    }
}

#ifdef CONFIG_RW_SPINLOCK

FAR static void *reader(FAR void *arg)
{
  irqstate_t flags;
  FAR clock_t *wait_time = (FAR clock_t *)arg;
  FAR struct tcb_s *tcb = this_task();

  flags = read_lock_irqsave(NULL);
  up_mdelay(MDELAY);
  read_unlock_irqrestore(NULL, flags);
  *wait_time = tcb->busywait_total;

  return NULL;
}

FAR static void *writer(FAR void *arg)
{
  irqstate_t flags;
  FAR clock_t *wait_time = (FAR clock_t *)arg;
  FAR struct tcb_s *tcb = this_task();

  flags = write_lock_irqsave(NULL);
  up_mdelay(MDELAY);
  write_unlock_irqrestore(NULL, flags);
  *wait_time = tcb->busywait_total;

  return NULL;
}

/**
 * Test the rw_spinlock.
 */

static void test_rw_spinlock(void **state)
{
  int            ret;
  cpu_set_t      cpuset;
  pthread_attr_t attr;
  pthread_t      thread[2];
  clock_t        wait_time[2];
  clock_t        time_all_read;
  clock_t        time_all_write;
  clock_t        time_read_write;

  /* Both threads perform read operations. */

  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  pthread_attr_init(&attr);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  ret = pthread_create(&thread[0], &attr, reader, &wait_time[0]);
  assert_int_equal(ret, 0);

  CPU_ZERO(&cpuset);
  CPU_SET(1, &cpuset);
  pthread_attr_init(&attr);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  ret = pthread_create(&thread[1], &attr, reader, &wait_time[1]);
  assert_int_equal(ret, 0);

  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);
  time_all_read = wait_time[0] + wait_time[1];

  /* Both threads perform write operations. */

  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  pthread_attr_init(&attr);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  ret = pthread_create(&thread[0], &attr, writer, &wait_time[0]);
  assert_int_equal(ret, 0);

  CPU_ZERO(&cpuset);
  CPU_SET(1, &cpuset);
  pthread_attr_init(&attr);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  ret = pthread_create(&thread[1], &attr, writer, &wait_time[1]);
  assert_int_equal(ret, 0);

  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);
  time_all_write = wait_time[0] + wait_time[1];

  /* One thread reads and the other thread writes. */

  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  pthread_attr_init(&attr);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  ret = pthread_create(&thread[0], &attr, reader, &wait_time[0]);
  assert_int_equal(ret, 0);

  CPU_ZERO(&cpuset);
  CPU_SET(1, &cpuset);
  pthread_attr_init(&attr);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  ret = pthread_create(&thread[1], &attr, writer, &wait_time[1]);
  assert_int_equal(ret, 0);

  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);
  time_read_write = wait_time[0] + wait_time[1];

  /* read + read <= write + write, read + read <= read + write */

  assert_true(time_all_read <= time_all_write);
  assert_true(time_all_read <= time_read_write);
}

#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_critical_section),
#ifdef CONFIG_RW_SPINLOCK
      cmocka_unit_test(test_rw_spinlock),
#endif
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
