/****************************************************************************
 * apps/testing/arch/irqnest/kmod/testisrs.c
 *
 * SPDX-License-Identifier: Apache-2.0
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

#include <assert.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include <nuttx/module.h>
#include <nuttx/lib/elf.h>

#ifdef CONFIG_TESTING_IRQNEST

#define IRQ_LO  CONFIG_TESTING_IRQNEST_IRQ_LO
#define IRQ_HI  CONFIG_TESTING_IRQNEST_IRQ_HI
#define PRI_LO  CONFIG_TESTING_IRQNEST_PRI_LO
#define PRI_HI  CONFIG_TESTING_IRQNEST_PRI_HI

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Variables
 ****************************************************************************/

static volatile bool g_irqtests_ready;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#if IRQ_LO > 0
static int test_isr_lo(int irq, void *context, void *arg)
{
#if defined(CONFIG_ARCH_HAVE_IRQTRIGGER) && IRQ_HI > 0
  up_trigger_irq(IRQ_HI, 0);
#endif

#ifndef CONFIG_ARCH_HIPRI_INTERRUPT
  up_irq_enable();    /* enable IRQ */
#endif

  /* do something lengthy handling here */

#ifndef CONFIG_ARCH_HIPRI_INTERRUPT
  up_irq_save();      /* disable IRQ */
#endif

  return OK;
}
#endif

#if IRQ_HI > 0
static int test_isr_hi(int irq, void *context, void *arg)
{
  return 0;
}
#endif

destructor_function void module_uninitialize(void)
{
  /* TODO: Check if there are any open references to the driver */

  syslog(LOG_INFO, "module_uninitialize\n");

#if IRQ_LO > 0
  up_disable_irq(IRQ_LO);
  irq_detach(IRQ_LO);
#endif

#if IRQ_HI > 0
  up_disable_irq(IRQ_HI);
  irq_detach(IRQ_HI);
#endif

  g_irqtests_ready = false;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: module_initialize
 *
 * Description:
 *   Register /dev/chardev
 *
 ****************************************************************************/

constructor_fuction void module_initialize(void)
{
  if (g_irqtests_ready)
    {
      return;
    }

  syslog(LOG_INFO, "module_initialize\n");

#if IRQ_LO > 0
  irq_attach(IRQ_LO, test_isr_lo, NULL);
  up_prioritize_irq(IRQ_LO, PRI_LO);
  up_enable_irq(IRQ_LO);
#endif

#if IRQ_HI > 0
  irq_attach(IRQ_HI, test_isr_hi, NULL);
  up_prioritize_irq(IRQ_HI, PRI_HI);
  up_enable_irq(IRQ_HI);
#endif

  g_irqtests_ready = true;
}

#endif
