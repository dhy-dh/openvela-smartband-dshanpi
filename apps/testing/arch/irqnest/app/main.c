/****************************************************************************
 * apps/testing/arch/irqnest/app/main.c
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
#include <stdio.h>

#include <nuttx/arch.h>
#include <arch/irq.h>

#include <nuttx/module.h>
#include <nuttx/symtab.h>

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

#define LOOP_COUNT        1000

#define IRQ_LO            CONFIG_TESTING_IRQNEST_IRQ_LO
#define IRQ_HI            CONFIG_TESTING_IRQNEST_IRQ_HI
#define PRI_LO            CONFIG_TESTING_IRQNEST_PRI_LO
#define PRI_HI            CONFIG_TESTING_IRQNEST_PRI_HI

#define IRQDBG_FILE       "/proc/irqs"

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * irqnest_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int i;
  FAR FILE *fp;
  int ret = 0;

  fp = fopen(IRQDBG_FILE, "wt");
  if (fp == NULL)
    {
      printf("Failed to open %s\n", IRQDBG_FILE);
      return -1;
    }

  printf("Test IRQ LO=%d(%d) HI=%d(%d)\n", IRQ_LO, PRI_LO, IRQ_HI, PRI_HI);

  /* trigger low priority IRQ only here */

  for (i = 0; i < LOOP_COUNT && ret >= 0; i++)
    {
      ret = fprintf(fp, "%d\n", IRQ_LO);
    }

  if (i >= LOOP_COUNT)
    {
      printf("Done %d times, see irqinfo please\n", LOOP_COUNT);
    }

  fclose(fp);
  return (ret >= 0) ? 0 : -2;
}
