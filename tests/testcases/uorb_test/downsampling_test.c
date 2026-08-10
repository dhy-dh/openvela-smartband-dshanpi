#include <errno.h>
#include <math.h>
#include <poll.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <uORB/uORB.h>
#include "utility.h"

static unsigned long g_main_interval;

static int advertise_with_interval(int argc, FAR char *argv[])
{
  const int max_publish = 100;
  const int queue_size = 8;
  struct orb_test_medium_s sample = {0, 0};
  int instance = 0;
  int i = 0;
  int fd;

  fd = orb_advertise_multi_queue_persist(ORB_ID(orb_test_medium_multi),
                                         NULL, &instance, queue_size);
  orb_set_interval(fd, g_main_interval);
  while (i++ < max_publish)
  {
    usleep(g_main_interval);

    sample.timestamp = orb_absolute_time();
    sample.val = i;

    test_note("---------------------------");
    test_note("publ:%llu val:%i", sample.timestamp, sample.val);
    orb_publish(ORB_ID(orb_test_medium_multi), fd, &sample);
  }

  orb_unadvertise(fd);
  return 0;
}

static int down_sample_test(unsigned long user1_interval,
                            unsigned long user2_interval)
{
  FAR char *const args[1] = {NULL};
  struct orb_test_medium_s sample;
  struct pollfd fds[2];
  int task;
  int ret;

  test_note("test down-sampling\n");
  test_note("main interval is %lu(us)\n", g_main_interval);
  test_note("user1 interval is %lu(us)\n", user1_interval);
  if (user2_interval)
  {
    test_note("user2 interval is %lu(us)\n", user2_interval);
  }

  fds[0].events = POLLIN;
  fds[0].fd = orb_subscribe(ORB_ID(orb_test_medium_multi));
  if (fds[0].fd < 0)
  {
    return test_fail("subscribe(1) failed, %d\n", fds[0].fd);
  }

  orb_set_interval(fds[0].fd, user1_interval);

  if (user2_interval)
  {
    fds[1].events = POLLIN;
    fds[1].fd = orb_subscribe(ORB_ID(orb_test_medium_multi));
    if (fds[1].fd < 0)
    {
      orb_unsubscribe(fds[0].fd);
      return test_fail("subscribe(2) failed, %d\n", fds[1].fd);
    }

    orb_set_interval(fds[1].fd, user2_interval);
  }
  else
  {
    fds[1].events = 0;
    fds[1].fd = -1;
  }

  task = task_create("down_sample_test",
                     SCHED_PRIORITY_MAX - 5,
                     CONFIG_UORB_STACKSIZE,
                     (main_t)&advertise_with_interval,
                     args);

  if (task < 0)
  {
    goto out;
  }

  while (poll(fds, 2, 3000) > 0)
  {
    if (fds[0].revents & POLLIN)
    {
      ret = orb_copy(ORB_ID(orb_test_medium_multi), fds[0].fd, &sample);
      if (ret < 0)
      {
        test_note("copy1:%llu fail\n", orb_absolute_time());
      }
      else
      {
        test_note("copy1:%llu val:%i", orb_absolute_time(), sample.val);
      }
    }

    if (fds[1].revents & POLLIN)
    {
      ret = orb_copy(ORB_ID(orb_test_medium_multi), fds[1].fd, &sample);
      if (ret < 0)
      {
        test_note("copy2:%llu fail\n", orb_absolute_time());
      }
      else
      {
        test_note("copy2:%llu val:%i", orb_absolute_time(), sample.val);
      }
    }
  }

out:
  orb_unsubscribe(fds[0].fd);
  if (user2_interval)
  {
    orb_unsubscribe(fds[1].fd);
  }

  return 0;
}

int main(int argc, FAR char *argv[])
{
  if (argc > 1)
  {
    unsigned long user1_interval = 20000;
    unsigned long user2_interval = 0;

    g_main_interval = 10000;
    if (argc > 2)
    {
      g_main_interval = atoi(argv[1]);
      user1_interval = atoi(argv[2]);
      if (!g_main_interval || !user1_interval)
      {
        goto out;
      }
    }

    if (argc > 3)
    {
      user2_interval = atoi(argv[3]);
      if (!user2_interval)
      {
        goto out;
      }
    }

    return down_sample_test(user1_interval, user2_interval);
  }

out:
  syslog(LOG_INFO, "Usage:\n");
  syslog(LOG_INFO, "uorb_downsample [main user1 [user2]\n");
  return -EINVAL;
}
