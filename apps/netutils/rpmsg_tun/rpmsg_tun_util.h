/****************************************************************************
 * apps/netutils/rpmsg_tun/rpmsg_tun_util.h
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

#ifndef __APPS_NETUTILS_RPMSG_TUN_RPMSG_TUN_UTIL_H
#define __APPS_NETUTILS_RPMSG_TUN_RPMSG_TUN_UTIL_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: rpmsg_tun_setup
 *
 * Description:
 *   Setup tun device
 *
 * Parameters:
 *   name - tun device name
 *   ip   - tun device ip
 *   mask - tun device mask
 *
 * Returned Value:
 *   int - tun fd on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_setup(const char *name, const char *ip, const char *mask);

/****************************************************************************
 * Name: rpmsg_tun_connect
 *
 * Description:
 *   Create rpmsg socket and connect to server
 *
 * Parameters:
 *   cpu  - remote cpu name
 *   name - tun device name
 *
 * Returned Value:
 *   int - rpmsg socket fd on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_connect(const char *cpu, const char *name);

/****************************************************************************
 * Name: rpmsg_tun_accept
 *
 * Description:
 *   Create rpmsg socket and accept the connection from client
 *
 * Parameters:
 *   name - tun device name
 *
 * Returned Value:
 *   int - rpmsg socket fd on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_accept(const char *name);

/****************************************************************************
 * Name: rpmsg_tun_connect_netlink
 *
 * Description:
 *   Create netlink socket and monitor the route mesage
 *
 * Parameters:
 *   None
 *
 * Returned Value:
 *   int - netlink fd on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_connect_netlink(void);

/****************************************************************************
 * Name: rpmsg_tun_wait_running
 *
 * Description:
 *   Wait network device running
 *
 * Parameters:
 *   fd   - netlink socket fd
 *   name - network device name
 *
 * Returned Value:
 *   1 is running, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_wait_running(int fd, const char *name);

/****************************************************************************
 * Name: rpmsg_tun_loop
 *
 * Description:
 *   Poll tunfd and rpmsgfd and nlfd when fds return POLLIN
 *   then read buffer from tunfd and send to rpmsgfd
 *   or read buffer from rpmsgfd and send to tunfd
 *
 * Parameters:
 *   tunfd   - tun device fd
 *   rpmsgfd - rpmsg socket fd
 *   nlfd    - netlink socket fd
 *   name    - network device name
 *
 * Returned Value:
 *   int - 0 on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_loop(int tunfd, int rpmsgfd, int nlfd, const char *name);

#ifdef __cplusplus
}
#endif
#undef EXTERN

#endif /* __APPS_NETUTILS_RPMSG_TUN_RPMSG_TUN_UTIL_H */
