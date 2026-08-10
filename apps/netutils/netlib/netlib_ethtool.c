/****************************************************************************
 * apps/netutils/netlib/netlib_ethtool.c
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

#include <sys/socket.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <debug.h>

#include <netinet/in.h>
#include <net/if.h>
#include <nuttx/ethtool.h>

#include "netutils/netlib.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: netlib_ethtool_ioctl
 *
 * Description:
 *   Perform ethtool-related IOCTL operations on network devices
 *
 * Parameters:
 *   ifname  - The name of the ethernet device
 *   data    - Pointer to an ethtool-related data structure
 *             (e.g., ethtool_cmd, ethtool_chns), used to store
 *             input/output parameters (specific type determined by @cmd)
 *
 * Return:
 *   0 on success; -1 on failure
 *
 ****************************************************************************/

int netlib_ethtool_ioctl(FAR const char *ifname, FAR void *data)
{
  struct ifreq req;
  int sockfd;
  int ret;

  if (ifname == NULL || data == NULL)
    {
      return -EINVAL;
    }

  sockfd = socket(AF_INET, SOCK_CTRL, 0);
  if (sockfd < 0)
    {
      nerr("Failed to create socket, cmd %" PRIx32 ", errno:%d",
           *(uint32_t *)data, errno);
      return sockfd;
    }

  /* Add the device name to the request */

  strlcpy(req.ifr_name, ifname, IFNAMSIZ);

  req.ifr_data = data;

  ret = ioctl(sockfd, SIOCETHTOOL, (unsigned long)&req);
  close(sockfd);

  return ret;
}

/****************************************************************************
 * Name: netlib_ethtool_getchannel
 *
 * Description:
 *   Ethernet device hardware may have multiple channels, and users can turn
 *   on/off the channels through the API. When the channel is open, data can
 *   be sent and received through the channel. When the channel is closed, no
 *   corresponding transmit/receive interrupt is found.
 *
 * Parameters:
 *   ifname   - The name of the ethernet device
 *   combined_chns_map - The status of channel opening or closing.  For
 *              example, 0x51 means that channel 0,4, and 6 will be turned
 *              on, and other channels will be turned off.
 *
 * Return:
 *   0 on success; -1 on failure
 *
 ****************************************************************************/

int netlib_ethtool_getchannel(FAR const char *ifname,
                              FAR uint32_t *combined_chns_map)
{
  struct ethtool_chns2 chns;
  int ret;

  /* Get device combined channel */

  chns.cmd = ETHTOOL_GCHANNELS2;

  ret = netlib_ethtool_ioctl(ifname, &chns);
  if (ret == OK)
    {
      *combined_chns_map = chns.combined_chns_map;
    }

  return ret;
}

/****************************************************************************
 * Name: netlib_ethtool_setchannel
 *
 * Description:
 *   Ethernet device hardware may have multiple channels, and users can turn
 *   on/off the channels through the API. When the channel is open, data can
 *   be sent and received through the channel. When the channel is closed, no
 *   corresponding transmit/receive interrupt is found.
 *
 * Parameters:
 *   ifname   - The name of the ethernet device
 *   combined_chns_map - The status of channel opening or closing is set in
 *              the form of bitmap.  For example, 0x51 means that channel
 *              0,4, and 6 will be turned on, and others will be turned off.
 *
 * Return:
 *   0 on success; -1 on failure
 *
 ****************************************************************************/

int netlib_ethtool_setchannel(FAR const char *ifname,
                              uint32_t combined_chns_map)
{
  struct ethtool_chns2 chns;

  /* Set device combined channel */

  chns.cmd = ETHTOOL_SCHANNELS2;
  chns.rx_chns_map = 0;
  chns.tx_chns_map = 0;
  chns.combined_chns_map = combined_chns_map;

  return netlib_ethtool_ioctl(ifname, &chns);
}
