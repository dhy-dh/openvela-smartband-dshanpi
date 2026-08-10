/****************************************************************************
 * apps/testing/mm/tlsftest/tlsftest_main.c
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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "tlsf/tlsf.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Default parameter values */

#define DEFAULT_POOL_SIZE       (64 * 1024 * 1024)
#define DEFAULT_NUM_TESTS       10

/* Fixed parameters for test cases */

#define NUM_ALLOCATIONS         1000
#define RETAIN_PERCENTAGE       60
#define NUM_LOOPS               10
#define OUTER_NUM_LOOPS         10000
#define SLEEP_TIME              1

/* Allocation size probability distribution */

#define PROB_4KB_OR_LESS        0.95
#define PROB_ABOVE_80KB         0.003
#define PROB_4KB_TO_80KB        (1 - PROB_4KB_OR_LESS - PROB_ABOVE_80KB)

/* Test case types */

#define TEST_CASE_LARGEST_FREE_BLK_SIZE          0

/****************************************************************************
 * Private Type Prototypes
 ****************************************************************************/

/* Memory context structure to manage all fragmentation test resources */

typedef struct memory_context_s
{
  void *pool;                              /* Memory pool buffer */
  size_t pool_size;                        /* Size of the memory pool */
  tlsf_t tlsf;                             /* Active TLSF instance */
  void *ptrs[NUM_LOOPS][NUM_ALLOCATIONS];  /* Array of allocated pointers */
} memory_context_t;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

void print_usage(const char *progname);

void find_max_free_block(void *ptr, size_t size, int used, void *user);

memory_context_t *create_fragmented_memory(size_t pool_size);

void destroy_fragmented_memory(memory_context_t *ctx);

int compare_largest_free_block_methods(memory_context_t *ctx);

int do_largest_free_blk_size_test(size_t pool_size);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

void print_usage(const char *progname)
{
  printf("Usage: %s [options]\n\n", progname);
  printf("TLSF Memory Allocator Test Tool\n\n");
  printf("Options:\n");

  printf("  -s, --poolsize=SIZE     Memory pool size in bytes "
         "(default: %d)\n", DEFAULT_POOL_SIZE);

  printf("  -t, --tests=NUM         Number of test iterations "
         "(default: %d)\n", DEFAULT_NUM_TESTS);

  printf("  -c, --testcase=CASE     Test case to run:\n");
  printf("                          0 = Largest Free Blk Size Test\n");
  printf("                          Other values are reserved for "
         "future test cases\n");
  printf("  -h, --help              Show this help message\n\n");
}

/* Callback function to find the maximum free block during pool walk */

void find_max_free_block(void *ptr, size_t size, int used, void *user)
{
  if (!used)
    {
      size_t *max_free_block = (size_t *)user;
      if (size > *max_free_block)
        {
          *max_free_block = size;
        }
    }
}

/* Creates a fragmented memory scenario in the specified pool */

memory_context_t *create_fragmented_memory(size_t pool_size)
{
  memory_context_t *ctx;
  int retain_target = 0;
  int retained_count = 0;
  int total_blocks = 0;
  int current_count = 0;

  ctx = malloc(sizeof(memory_context_t));
  if (!ctx)
    {
      fprintf(stderr, "Failed to allocate memory context.\n");
      return NULL;
    }

  ctx->pool_size = pool_size;
  ctx->pool = malloc(pool_size);
  if (!ctx->pool)
    {
      fprintf(stderr, "Failed to allocate memory pool.\n");
      free(ctx);
      return NULL;
    }

  /* Initialize TLSF memory manager with the pool */

  ctx->tlsf = tlsf_create_with_pool(ctx->pool, pool_size);
  if (!ctx->tlsf)
    {
      fprintf(stderr, "Failed to create TLSF memory manager.\n");
      free(ctx->pool);
      free(ctx);
      return NULL;
    }

  memset(ctx->ptrs, 0, sizeof(ctx->ptrs));

  srand((unsigned int)time(NULL));

  /* Construct fragmentation scenario
   * through random allocations and releases
   */

  for (int out_loop = 0; out_loop < OUTER_NUM_LOOPS; ++out_loop)
    {
      for (int loop = 0; loop < NUM_LOOPS; ++loop)
        {
          for (int step = 0; step < NUM_ALLOCATIONS; ++step)
            {
              /* 70% probability to allocate memory */

              if (rand() % 100 < 70)
                {
                  double prob = (double)rand() / RAND_MAX;
                  size_t alloc_size = 0;

                  /* Determine allocation size
                   * based on probability distribution
                   */

                  if (prob < PROB_4KB_OR_LESS)
                    {
                      alloc_size = (rand() % 4096) + 512;          /* 512B - 4KB */
                    }
                  else if (prob < PROB_4KB_OR_LESS + PROB_4KB_TO_80KB)
                    {
                      alloc_size = (rand() % (80000 - 4096)) + 4096; /* 4KB - 80KB */
                    }
                  else
                    {
                      alloc_size = (rand() % (450 * 1024 - 80000)) + 80000; /* 80KB - 450KB */
                    }

                  /* If slot already has memory, free it first */

                  if (ctx->ptrs[loop][step])
                    {
                      tlsf_free(ctx->tlsf, ctx->ptrs[loop][step]);
                    }

                  /* Allocate new memory block */

                  ctx->ptrs[loop][step] = tlsf_malloc(ctx->tlsf, alloc_size);
                  if (!ctx->ptrs[loop][step])
                    {
                      fprintf(stderr, "Failed to allocate %zu bytes.\n",
                              alloc_size);
                      return NULL;
                    }
                }
              else  /* 30% probability to release a random block */
                {
                  int release_loop = rand() % (loop + 1);
                  int release_step = rand() % (step + 1);
                  if (ctx->ptrs[release_loop][release_step])
                    {
                      tlsf_free(ctx->tlsf,
                                ctx->ptrs[release_loop][release_step]);
                      ctx->ptrs[release_loop][release_step] = NULL;
                    }
                }
            }
        }
    }

  /* Retain specified percentage of memory blocks */

  retain_target = NUM_LOOPS * NUM_ALLOCATIONS * RETAIN_PERCENTAGE / 100;

  /* First pass: count currently allocated blocks */

  for (int loop = 0; loop < NUM_LOOPS; ++loop)
    {
      for (int i = 0; i < NUM_ALLOCATIONS; ++i)
        {
          if (ctx->ptrs[loop][i]) total_blocks++;
        }
    }

  /* Second pass: mark blocks to retain */

  for (int loop = 0;
       loop < NUM_LOOPS && retained_count < retain_target;
       ++loop)
    {
      for (int i = 0;
           i < NUM_ALLOCATIONS && retained_count < retain_target;
           ++i)
        {
          if (ctx->ptrs[loop][i])
            {
              retained_count++;
            }
        }
    }

  /* Third pass: release non-retained blocks */

  for (int loop = 0; loop < NUM_LOOPS; ++loop)
    {
      for (int i = 0; i < NUM_ALLOCATIONS; ++i)
        {
          if (ctx->ptrs[loop][i])
            {
              current_count++;
              if (current_count > retain_target)
                {
                  tlsf_free(ctx->tlsf, ctx->ptrs[loop][i]);
                  ctx->ptrs[loop][i] = NULL;
                }
            }
        }
    }

  return ctx;
}

/* Destroys the fragmented memory context and releases all resources */

void destroy_fragmented_memory(memory_context_t *ctx)
{
  if (!ctx) return;

  if (ctx->tlsf)
    {
      /* Release all retained memory blocks */

      for (int loop = 0; loop < NUM_LOOPS; ++loop)
        {
          for (int i = 0; i < NUM_ALLOCATIONS; ++i)
            {
              if (ctx->ptrs[loop][i])
                {
                  tlsf_free(ctx->tlsf, ctx->ptrs[loop][i]);
                }
            }
        }

      /* Destroy TLSF memory manager */

      tlsf_destroy(ctx->tlsf);
    }

  /* Free the memory pool and context structure */

  if (ctx->pool) free(ctx->pool);
  free(ctx);
}

int compare_largest_free_block_methods(memory_context_t *ctx)
{
  size_t walk_pool_result = 0;
  size_t api_result;

  if (!ctx || !ctx->tlsf)
    {
      fprintf(stderr, "Invalid context for consistency check.\n");
      return 1;
    }

  tlsf_walk_pool(tlsf_get_pool(ctx->tlsf), find_max_free_block,
                 &walk_pool_result);
  api_result = tlsf_largest_free_block(ctx->tlsf);

  if (walk_pool_result != api_result)
    {
      fprintf(stderr, "Consistency check failed: walk_pool=%zu, API=%zu\n",
              walk_pool_result, api_result);
      return 1;
    }

  return 0;
}

int do_largest_free_blk_size_test(size_t pool_size)
{
  int result;
  memory_context_t *ctx;

  ctx = create_fragmented_memory(pool_size);
  if (!ctx)
    {
      fprintf(stderr, "Failed to create fragmented memory scenario.\n");
      return 1;
    }

  result = compare_largest_free_block_methods(ctx);
  destroy_fragmented_memory(ctx);

  return result;
}

int main(int argc, char *argv[])
{
  int opt;
  int result = 0;
  size_t pool_size = DEFAULT_POOL_SIZE;
  int num_tests = DEFAULT_NUM_TESTS;
  int test_case = TEST_CASE_LARGEST_FREE_BLK_SIZE;

  static struct option long_options[] =
  {
    {"poolsize", required_argument, 0, 's'},
    {"tests", required_argument, 0, 't'},
    {"testcase", required_argument, 0, 'c'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv, "s:t:c:h",
    long_options, NULL)) != -1)
    {
      switch (opt)
        {
          case 's':
            pool_size = strtoul(optarg, NULL, 10);
            break;
          case 't':
            num_tests = atoi(optarg);
            break;
          case 'c':
            test_case = atoi(optarg);
            break;
          case 'h':
            print_usage(argv[0]);
            return 0;
          default:
            print_usage(argv[0]);
            return 1;
        }
    }

  for (int i = 0; i < num_tests; ++i)
    {
      printf("Running test %d/%d...\n", i + 1, num_tests);

      switch (test_case)
        {
          case TEST_CASE_LARGEST_FREE_BLK_SIZE:
            result = do_largest_free_blk_size_test(pool_size);
            break;

          default:
            fprintf(stderr, "Invalid test case: %d\n", test_case);
            return 1;
        }

      if (result != 0)
        {
          fprintf(stderr, "Test failed on iteration %d.\n", i + 1);
          break;
        }

      if (i < num_tests - 1)
        {
          sleep(SLEEP_TIME);
        }
    }

  if (result == 0)
    {
      printf("All tests passed.\n");
    }
  else
    {
      printf("Some tests failed.\n");
    }

  return result;
}
