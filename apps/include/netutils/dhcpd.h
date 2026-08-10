/****************************************************************************
 * apps/include/netutils/dhcpd.h
 *
 *   Copyright (C) 2007, 2009, 2011, 2015 Gregory Nutt. All rights reserved.
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

#ifndef __APPS_INCLUDE_NETUTILS_DHCPD_H
#define __APPS_INCLUDE_NETUTILS_DHCPD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <netinet/in.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

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
 *   The core event loop and main logic of the DHCP server daemon.
 *
 *   This function is the heart of the DHCPD. It is a long-running, blocking
 *   function that performs the following steps:
 *   1. Initializes the daemon's state, including allocating memory and
 *   setting up a signal handler to gracefully handle stop requests.
 *   2. Enters an loop that is only broken when a stop is requested.
 *   3. Inside the loop, it listens for incoming DHCP client packets on a
 *   socket.
 *   4. Upon receiving a packet, it parses the message and dispatches it to
 *   the appropriate handler.
 *   5. When the loop terminates, it performs cleanup by freeing allocated
 *   resources.
 *
 * @param interface The name of the network interface to listen on
 *   (e.g., "eth0").
 *
 * @return
 *   OK (0) on successful shutdown, or a negated errno value if an
 *   unrecoverable error occurred.
 */

int dhcpd_run(FAR const char *interface);

/**
 * @brief
 *   Start the DHCPD daemon as a background task.
 *
 *   This function creates a new task that runs the main DHCPD logic.
 *   It checks if the daemon is already running to prevent
 *   multiple instances.
 *
 * @param interface The name of the network interface for the daemon to
 *   listen on.
 *
 * @return
 *   OK (0) on success. A negated errno value is returned on failure.
 */

int dhcpd_start(FAR const char *interface);

/**
 * @brief
 *   Stop a running DHCPD daemon.
 *
 *   This function gracefully shuts down the DHCPD daemon by:
 *   1. Finding the process ID (PID) of the running daemon.
 *   2. Sending a specific signal (CONFIG_NETUTILS_DHCPD_SIGWAKEUP) to it.
 *   3. Waiting for the daemon task to completely terminate and clean up.
 *
 * @return
 *   OK (0) on success. A negated errno value is returned on failure.
 */

int dhcpd_stop(void);

/**
 * @brief
 *   Set the starting IP address for the DHCPD's address lease pool.
 *
 * @param startip The first IP address in the range to be leased to clients.
 *
 * @return Always returns OK (0).
 */

int dhcpd_set_startip(in_addr_t startip);

/**
 * @brief
 *   Set the router (gateway) IP address to be provided to clients.
 *
 * @param routerip The IP address of the default router.
 *
 * @return Always returns OK (0).
 */

int dhcpd_set_routerip(in_addr_t routerip);

/**
 * @brief
 *   Set the subnet mask to be provided to clients.
 *
 * @param netmask The subnet mask.
 *
 * @return Always returns OK (0).
 */

int dhcpd_set_netmask(in_addr_t netmask);

/**
 * @brief
 *   Set the primary DNS server IP address to be provided to clients.
 *
 * @param dnsip The IP address of the DNS server.
 *
 * @return Always returns OK (0).
 */

int dhcpd_set_dnsip(in_addr_t dnsip);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __APPS_INCLUDE_NETUTILS_DHCPD_H */
