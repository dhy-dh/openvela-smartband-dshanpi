/****************************************************************************
 * apps/include/netutils/dhcp6c.h
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

#ifndef __APPS_INCLUDE_NETUTILS_DHCP6C_H
#define __APPS_INCLUDE_NETUTILS_DHCP6C_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct dhcp6c_state
{
  struct in6_addr addr;
  struct in6_addr pd;       /* prefix address */
  struct in6_addr dns;
  struct in6_addr netmask;
  uint8_t pl;               /* prefix len */
  uint32_t t1;
  uint32_t t2;
};

typedef void (*dhcp6c_callback_t)(FAR struct dhcp6c_state *presult);

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * @brief
 *   Initialize a DHCPv6 client session on a specific network interface.
 *
 *   This function creates and initializes a new DHCPv6 client instance on a
 *   specified network interface. It is a convenience wrapper for
 *   dhcp6c_precise_open(), pre-configuring the session with common suitable
 *   for a standard address acquisition attempt.
 *
 * @param interface The name of the network interface to run the client on
 *   (e.g., "eth0").
 *
 * @return
 *   A non-NULL handle on success, which must be used in subsequent API
 *   calls. Returns NULL on failure to allocate or initialize the
 *   session.
 */

FAR void *dhcp6c_open(FAR const char *interface);

/**
 * @brief
 *   Perform a synchronous DHCPv6 request to obtain a lease.
 *
 *   This function executes a full, blocking DHCPv6 transaction to acquire
 *   an address and other configuration data.
 *
 * @param handle  The DHCPv6 client session handle, previously obtained from
 *   dhcp6c_open().
 * @param presult A pointer to a dhcp6c_state structure where the resulting
 *   lease information will be stored upon success.
 *
 * @return
 *   OK (0) on success (a lease was acquired).
 *   ERROR (-1) on failure (e.g., invalid handle, timeout, or no lease
 *   offered).
 */

int dhcp6c_request(FAR void *handle, FAR struct dhcp6c_state *presult);

/**
 * @brief
 *   Asynchronously start the DHCPv6 address acquisition process.
 *
 *   This function initiates the DHCPv6 negotiation in a new background
 *   thread, returning immediately without blocking. The result of the
 *   negotiation will be delivered via the provided callback function.
 *
 * @param handle   The DHCPv6 client session handle, previously created.
 * @param callback A function to be called upon completion of the DHCPv6
 *   negotiation.
 *
 * @return
 *   OK (0) if the background thread was successfully started;
 *   ERROR (-1) on failure. A successful return only indicates that the
 *   process has begun, not that an address has been acquired.
 */

int dhcp6c_request_async(FAR void *handle, dhcp6c_callback_t callback);

/**
 * @brief
 *   Cancel an ongoing DHCPv6 process.
 *
 *   This function stops a DHCPv6 client operation. For an asynchronous
 *   request, it signals the background thread to terminate and waits
 *   for it to exit.
 *
 * @param handle The DHCPv6 client session handle for the operation to be
 *   cancelled.
 */

void dhcp6c_cancel(FAR void *handle);

/**
 * @brief
 *   Close a DHCPv6 client session and releases all associated resources.
 *
 *   This function ensures a clean shutdown of the DHCPv6 session by:
 *   1. Cancelling any active background thread by calling dhcp6c_cancel().
 *   2. Closing file descriptors.
 *   3. Freeing all dynamically allocated memory, including internal state
 *   data and the session handle itself.
 *
 *   After this function returns, the handle becomes invalid and must not
 *   be used again.
 *
 * @param handle The DHCPv6 client session handle to close.
 */

void dhcp6c_close(FAR void *handle);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __APPS_INCLUDE_NETUTILS_DHCP6C_H */
