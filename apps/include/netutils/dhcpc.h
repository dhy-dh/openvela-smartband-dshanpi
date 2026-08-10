/****************************************************************************
 * apps/include/netutils/dhcpc.h
 *
 *   Copyright (C) 2007, 2009-2011, 2015 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * This logic was leveraged from uIP which also has a BSD-style license:
 *
 *   Copyright (c) 2005, Swedish Institute of Computer Science
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __APPS_INCLUDE_NETUTILS_DHCPC_H
#define __APPS_INCLUDE_NETUTILS_DHCPC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <netinet/in.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct dhcpc_state
{
  struct in_addr serverid;
  struct in_addr ipaddr;
  struct in_addr netmask;
  struct in_addr dnsaddr;
  struct in_addr default_router;
  uint32_t       lease_time;      /* Lease expires in this number of seconds */
};

typedef void (*dhcpc_callback_t)(FAR struct dhcpc_state *presult);

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
 *   Initialize a DHCP client session on a network interface.
 *
 *   This function allocates a state structure and configures a UDP socket
 *   for subsequent DHCP client operations.
 *
 * @param interface The name of the network interface to use (e.g., "eth0").
 * @param macaddr   A pointer to the MAC address of the interface.
 * @param maclen    The length of the MAC address in bytes.
 *
 * @return
 *   A non-NULL handle to the new DHCP client session on success;
 *   NULL is returned on failure (e.g., memory allocation or socket error).
 */

FAR void *dhcpc_open(FAR const char *interface,
                     FAR const void *mac_addr, int mac_len);

/**
 * @brief
 *   Perform the DHCP negotiation to obtain an IP address and network
 *   lease.
 *
 *   This function executes the core DHCP client logic to acquire a valid
 *   network configuration from a DHCP server. It is a blocking an call.
 *
 * @param handle   The DHCP client session handle, previously created by
 *   dhcpc_open().
 * @param presult  An output pointer to a structure that will be populated
 *   with the lease details on success.
 *
 * @return
 *   OK (0) on success; ERROR (-1) on failure.
 */

int  dhcpc_request(FAR void *handle, FAR struct dhcpc_state *presult);

/**
 * @brief
 *   Start the DHCP request process asynchronously.
 *
 *   This function initiates the DHCP negotiation in a new background
 *   thread, returning immediately without blocking. The result of the
 *   negotiation will be delivered via the provided callback function.
 *
 * @param handle   The DHCP client session handle, previously created by
 *   dhcpc_open().
 * @param callback A function to be called upon completion of the DHCP
 *   negotiation.
 *
 * @return
 *   OK (0) if the background thread was successfully started;
 *   ERROR (-1) on failure. A successful return only indicates that the
 *   process has begun, not that a lease has been acquired.
 */

int  dhcpc_request_async(FAR void *handle, dhcpc_callback_t callback);

/**
 * @brief
 *   Cancel an ongoing DHCP negotiation process.
 *
 *   This function is used to stop a DHCP client operation.
 *   It works by:
 *   1. Setting an internal cancellation flag for synchronous operations.
 *   2. Signaling and then joining the background thread for asynchronous
 *   operations.
 *
 * @param handle The DHCP client session handle for the operation to be
 *   cancelled.
 */

void dhcpc_cancel(FAR void *handle);

/**
 * @brief
 *   Close a DHCP client session and releases all associated resources.
 *
 *   This function serves as the counterpart to dhcpc_open(). It ensures a
 *   clean shutdown of the DHCP session by:
 *   1. Cancelling any active background thread.
 *   2. Closing the network socket.
 *   3. Freeing the memory allocated for the session handle.
 *
 *   After this function returns, the handle becomes invalid and must not
 *   be used again.
 *
 * @param handle The DHCP client session handle to close.
 */

void dhcpc_close(FAR void *handle);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __APPS_INCLUDE_NETUTILS_DHCPC_H */
