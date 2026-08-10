/****************************************************************************
 * apps/include/netutils/netlib.h
 * Various non-standard APIs to support netutils.  All non-standard and
 * intended only for internal use.
 *
 *   Copyright (C) 2007, 2009, 2011, 2015, 2017 Gregory Nutt. All rights
 *   reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Some of these APIs derive from uIP.  uIP also has a BSD style license:
 *
 *   Author: Adam Dunkels <adam@sics.se>
 *   Copyright (c) 2002, Adam Dunkels.
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __APPS_INCLUDE_NETUTILS_NETLIB_H
#define __APPS_INCLUDE_NETUTILS_NETLIB_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/socket.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include <net/if.h>
#include <netinet/in.h>
#include <nuttx/mm/iob.h>
#include <nuttx/net/netdev.h>
#include <nuttx/net/netconfig.h>

#ifdef CONFIG_NET_IPTABLES
#  include <nuttx/net/netfilter/ip_tables.h>
#  include <nuttx/net/netfilter/ip6_tables.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#undef HAVE_ROUTE_PROCFS
#if !defined(CONFIG_DISABLE_MOUNTPOINT) && defined(CONFIG_NET_ROUTE) && \
    defined(CONFIG_FS_PROCFS) && !defined(CONFIG_FS_PROCFS_EXCLUDE_ROUTE)
#  define HAVE_ROUTE_PROCFS
#endif

#ifdef HAVE_ROUTE_PROCFS
#  ifndef CONFIG_NETLIB_PROCFS_MOUNTPT
#    define CONFIG_NETLIB_PROCFS_MOUNTPT "/proc"
#  endif

#  define IPv4_ROUTE_PATH CONFIG_NETLIB_PROCFS_MOUNTPT "/net/route/ipv4"
#  define IPv6_ROUTE_PATH CONFIG_NETLIB_PROCFS_MOUNTPT "/net/route/ipv6"
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifdef HAVE_ROUTE_PROCFS
#ifdef CONFIG_NET_IPv4
/**
 * @cond
 * Describes one entry from the IPv4 routing table.  All addresses are in
 * host byte order!
 */

struct netlib_ipv4_route_s
{
  in_addr_t prefix;               /* Routing prefix */
  in_addr_t netmask;              /* Routing netmask */
  in_addr_t router;               /* Router IPv4 address */
};
#endif

#ifdef CONFIG_NET_IPv6
/* Describes one entry from the IPv6 routing table.  All addresses are in
 * host byte order!
 */

struct netlib_ipv6_route_s
{
  uint16_t prefix[8];             /* Routing prefix */
  uint16_t netmask[8];            /* Routing netmask */
  uint16_t router[8];             /* Router IPv6 address */
};
#endif
#endif /* HAVE_ROUTE_PROCFS */

#ifdef CONFIG_NETLINK_ROUTE
/* Describes one device returned by netlib_get_devices() */

struct netlib_device_s
{
#ifdef CONFIG_NETDEV_IFINDEX
  uint8_t ifindex;                /* Interface index */
#endif
  char ifname[IFNAMSIZ];          /* Interface name */
};
#endif /* CONFIG_NETLINK_ROUTE*/

#ifdef CONFIG_NETLINK_NETFILTER
/* Describes one connection returned by netlib_get_conntrack() */

union netlib_conntrack_addr_u
{
#ifdef CONFIG_NET_IPv4
  struct in_addr ipv4;
#endif
#ifdef CONFIG_NET_IPv6
  struct in6_addr ipv6;
#endif
};

struct netlib_conntrack_tuple_s
{
  union netlib_conntrack_addr_u src;
  union netlib_conntrack_addr_u dst;

  union
  {
    struct
    {
      uint16_t sport;
      uint16_t dport;
    } tcp; /* and udp */

    struct
    {
      uint16_t id;
      uint8_t  type;
      uint8_t  code;
    } icmp; /* and icmp6 */
  } l4;

  uint8_t l4proto;
};

struct netlib_conntrack_s
{
  struct netlib_conntrack_tuple_s orig;
  struct netlib_conntrack_tuple_s reply;

  sa_family_t family; /* AF_INET or AF_INET6 */
  uint8_t     type;   /* IPCTNL_MSG_CT_* */
};

/* There might be many conntrack entries, so we don't use array of data, but
 * use callback instead.
 */

typedef CODE int (*netlib_conntrack_cb_t)(FAR struct netlib_conntrack_s *ct);

#endif /* CONFIG_NETLINK_NETFILTER */

#ifdef CONFIG_NETUTILS_NETLIB_GENERICURLPARSER
struct url_s
{
  FAR char *scheme;
  int       schemelen;
#if 0 /* not yet */
  FAR char *user;
  int       userlen;
  FAR char *password;
  int       passwordlen;
#endif
  FAR char *host;
  int       hostlen;
  uint16_t  port;
  FAR char *path;
  int       pathlen;
#if 0 /* not yet */
  FAR char *parameters;
  int       parameterslen;
  FAR char *bookmark;
  int       bookmarklen;
#endif
};
#endif

/****************************************************************************
 * Public Data
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

#ifdef CONFIG_NETLINK_ROUTE
/* Return a list of all devices */

/**
 * @endcond
 * @brief
 *   Return a list of all active network devices (devices in the DOWN state
 *   are not reported).
 *
 * @param  devlist    The location to store the list of devices.
 * @param  nentries   The size of the provided 'devlist' in number of
 *   entries each of size sizeof(struct netlib_device_s)
 * @param  family     Address family.  See AF_* definitions in
 *   sys/socket.h. Use AF_PACKET to return devices for all address
 *   families.
 *
 * @return
 *   The number of device entries read is returned on success; a negated
 *   errno value is returned on failure.
 */

ssize_t netlib_get_devices(FAR struct netlib_device_s *devlist,
                           unsigned int nentries, sa_family_t family);
#endif

#ifdef CONFIG_NETDEV_ETHTOOL_IOCTL

/**
 * @brief
 *   Perform ethtool-related IOCTL operations on network devices
 *
 * @param ifname    The name of the ethernet device
 * @param data      Pointer to an ethtool-related data structure
 *   (e.g., ethtool_cmd, ethtool_chns), used to store
 *   input/output parameters (specific type determined by @cmd)
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_ethtool_ioctl(FAR const char *ifname, FAR void *data);

/**
 * @brief
 *   Ethernet device hardware may have multiple channels, and users can turn
 *   on/off the channels through the API. When the channel is open, data can
 *   be sent and received through the channel. When the channel is closed, no
 *   corresponding transmit/receive interrupt is found.
 *
 * @param ifname            The name of the ethernet device
 * @param combined_chns_map The status of channel opening or closing.  For
 *   example, 0x51 means that channel 0,4, and 6 will be turned
 *   on, and other channels will be turned off.
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_ethtool_getchannel(FAR const char *ifname,
                              FAR uint32_t *combined_chns_map);

/**
 * @brief
 *   Ethernet device hardware may have multiple channels, and users can turn
 *   on/off the channels through the API. When the channel is open, data can
 *   be sent and received through the channel. When the channel is closed, no
 *   corresponding transmit/receive interrupt is found.
 *
 * @param ifname            The name of the ethernet device
 * @param combined_chns_map The status of channel opening or closing is set
 *   in the form of bitmap.  For example, 0x51 means that channel
 *   0,4, and 6 will be turned on, and others will be turned off.
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_ethtool_setchannel(FAR const char *ifname,
                              uint32_t combined_chns_map);
#endif

/**
 * @brief
 *   Convert a textual representation of an IP address to a numerical
 *   representation.
 *
 *   This function takes a textual representation of an IP address in
 *   the form a.b.c.d and converts it into a 4-byte array that can be
 *   used by other uIP functions.
 *
 *   addrstr A pointer to a string containing the IP address in
 *   textual form.
 *
 *   addr A pointer to a 4-byte array that will be filled in with
 *   the numerical representation of the address.
 *
 * @return
 *   0 If the IP address could not be parsed.
 *   Non-zero If the IP address was parsed.
 */

bool netlib_ipv4addrconv(FAR const char *addrstr, FAR uint8_t *addr);

/**
 * @brief
 *   Convert a textual representation of an Ethernet address to a numerical
 *   representation.
 *
 * @return
 *   0 If the IP address could not be parsed.
 *   Non-zero If the IP address was parsed.
 */

bool netlib_ethaddrconv(FAR const char *hwstr, FAR uint8_t *hw);

#ifdef CONFIG_NET_ETHERNET
/* Get and set IP/MAC addresses (Ethernet L2 only) */

/**
 * @brief
 *   Set the network driver MAC address
 *
 * @param  ifname   The name of the interface to use
 * @param  macaddr  MAC address to set, size must be IFHWADDRLEN
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_setmacaddr(FAR const char *ifname, FAR const uint8_t *macaddr);

/**
 * @brief
 *   Get the network driver IP address
 *
 * @param  ifname   The name of the interface to use
 * @param  macaddr  The location to return the MAC address
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_getmacaddr(FAR const char *ifname, FAR uint8_t *macaddr);
#endif

#ifdef CONFIG_WIRELESS_IEEE802154
/* IEEE 802.15.4 MAC IOCTL commands. */

/**
 * @brief
 *   Set the IEEE802.15.4 extended MAC address
 *
 * @param  ifname The name of the interface to use
 * @param  eaddr  The new extended address
 *
 * @return
 *   0 on success; -1 on failure.  errno will be set on failure.
 */

int netlib_seteaddr(FAR const char *ifname, FAR const uint8_t *eaddr);

/**
 * @brief
 *   Return the current PAN ID
 *
 * @param  ifname The name of the interface to use
 * @param  panid  The location to return the current PAN ID
 *
 * @return
 *   0 on success; -1 on failure.  errno will be set on failure.
 */

int netlib_getpanid(FAR const char *ifname, FAR uint8_t *panid);

/**
 * @brief
 *   Convert a 2-byte (16-bit) short address from its string
 *   representation into a binary byte array.
 *
 * @param hwstr A pointer to the null-terminated string to be converted.
 * @param hw    A pointer to a 2-byte buffer where the resulting binary
 *   address will be stored.
 *
 * @return
 *   0 If the IP address could not be parsed.
 *   Non-zero If the IP address was parsed.
 */

bool netlib_saddrconv(FAR const char *hwstr, FAR uint8_t *hw);

/**
 * @brief
 *   Convert an 8-byte (64-bit) extended address from its string
 *   representation into a binary byte array.
 *
 * @param hwstr A pointer to the null-terminated string to be converted.
 * @param hw    A pointer to an 8-byte buffer where the resulting binary
 *     address will be stored.
 *
 * @return
 *   0 If the IP address could not be parsed.
 *   Non-zero If the IP address was parsed.
 */

bool netlib_eaddrconv(FAR const char *hwstr, FAR uint8_t *hw);
#endif

#ifdef CONFIG_WIRELESS_PKTRADIO
/* IEEE 802.15.4 MAC IOCTL commands. */

struct pktradio_properties_s; /* Forward reference */
struct pktradio_addr_s;       /* Forward reference */

/**
 * @brief
 *   Get the non-IEEE802.15.4 packet radio node address
 *
 * @param  ifname  The name of the interface to use
 * @param  nodeadd Location to return the node address
 *
 * @return
 *   0 on success; -1 on failure.  errno will be set on failure.
 */

int netlib_getproperties(FAR const char *ifname,
                         FAR struct pktradio_properties_s *properties);

/**
 * @brief
 *   Set the non-IEEE802.15.4 packet radio node address
 *
 * @param  ifname  The name of the interface to use
 * @param  nodeadd The new node address
 *
 * @return
 *   0 on success; -1 on failure.  errno will be set on failure.
 */

int netlib_setnodeaddr(FAR const char *ifname,
                       FAR const struct pktradio_addr_s *nodeaddr);

/**
 * @brief
 *   Get the non-IEEE802.15.4 packet radio node address
 *
 * @param  ifname  The name of the interface to use
 * @param  nodeadd Location to return the node address
 *
 * @return
 *   0 on success; -1 on failure.  errno will be set on failure.
 */

int netlib_getnodnodeaddr(FAR const char *ifname,
                          FAR struct pktradio_addr_s *nodeaddr);

/**
 * @brief
 *   Get the non-IEEE802.15.4 packet radio node address
 *
 * @param  addrstr A string representing the node address
 * @param  nodeadd Location to return the node address
 *
 * @return
 *   true on success; false on failure.
 */

bool netlib_nodeaddrconv(FAR const char *addrstr,
                         FAR struct pktradio_addr_s *nodeaddr);
#endif

/* IP address support */

#ifdef CONFIG_NET_IPv4

/**
 * @brief
 *   Get the network driver IPv4 address
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The location to return the IP address
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_get_ipv4addr(FAR const char *ifname, FAR struct in_addr *addr);

/**
 * @brief
 *   Set the network driver IPv4 address
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The address to set
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_set_ipv4addr(FAR const char *ifname,
                        FAR const struct in_addr *addr);

/**
 * @brief
 *   Set the default router IPv4 address
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The address to set
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_set_dripv4addr(FAR const char *ifname,
                          FAR const struct in_addr *addr);

/**
 * @brief
 *   Get the network driver IPv4 default router address
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The location to return the default router address
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_get_dripv4addr(FAR const char *ifname, FAR struct in_addr *addr);

/**
 * @brief
 *   Set the PIv4 netmask
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The address to set
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_set_ipv4netmask(FAR const char *ifname,
                           FAR const struct in_addr *addr);

/**
 * @brief
 *   Get the network driver IPv4 netmask
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The location to return the netmask
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_get_ipv4netmask(FAR const char *ifname, FAR struct in_addr *addr);

/**
 * @brief
 *   Given the destination address, destipaddr, return the IP address
 *   assigned to the network adaptor that connects the sub-net that
 *   includes destipaddr.
 *
 *   If routing table support is enabled, then this logic will account for
 *   the case where the destination address is not locally accessible.  In
 *   this case, it will return the IP address of the network adaptor that
 *   provides the correct router to handle that destination address.
 *
 * @param  destipaddr The destination IPv4 address
 * @param  srcipaddr  The location to return that adaptor address that
 *   serves the sub-net that includes the destination address.
 *
 * @return
 *   Zero (OK) is returned on success with srcipaddr valid.  A negated
 *   errno value is returned on any failure and in this case the srcipaddr
 *   is not valid.
 */

int netlib_ipv4adaptor(in_addr_t destipaddr, FAR in_addr_t *srcipaddr);
#endif

/* We support multiple IPv6 addresses on a single interface.
 * Recommend to use netlib_add/del_ipv6addr to manage them, by which you
 * don't need to care about the slot it stored.
 *
 * Previous interfaces can still work, the ifname can be <eth>:<num>,
 * e.g. eth0:0 stands for managing the secondary address on eth0
 */

#ifdef CONFIG_NET_IPv6
#  ifdef CONFIG_NETDEV_MULTIPLE_IPv6

/**
 * @brief
 *   Add an IPv6 address to the network driver
 *
 * @param  ifname  The name of the interface to use
 * @param  ipaddr  The address to add
 * @param  preflen The prefix length of the address
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_add_ipv6addr(FAR const char *ifname,
                        FAR const struct in6_addr *addr, uint8_t preflen);

/**
 * @brief
 *   Remove an IPv6 address from the network driver
 *
 * @param  ifname  The name of the interface to use
 * @param  ipaddr  The address to delete
 * @param  preflen The prefix length of the address
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_del_ipv6addr(FAR const char *ifname,
                        FAR const struct in6_addr *addr, uint8_t preflen);
#  endif

/**
 * @brief
 *   Get the network driver IPv6 address
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The location to return the IP address
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_get_ipv6addr(FAR const char *ifname, FAR struct in6_addr *addr);

/**
 * @brief
 *   Set the network driver IPv6 address
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The address to set
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_set_ipv6addr(FAR const char *ifname,
                        FAR const struct in6_addr *addr);

/**
 * @brief
 *   Set the default router IP address
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The address to set
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_set_dripv6addr(FAR const char *ifname,
                          FAR const struct in6_addr *addr);

/**
 * @brief
 *   Set the netmask
 *
 * @param  ifname The name of the interface to use
 * @param  ipaddr The address to set
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_set_ipv6netmask(FAR const char *ifname,
                           FAR const struct in6_addr *addr);

/**
 * @brief
 *   Given the destination address, destipaddr, return the IP address
 *   assigned to the network adaptor that connects the sub-net that
 *   includes destipaddr.
 *
 *   If routing table support is enabled, then this logic will account for
 *   the case where the destination address is not locally accessible.  In
 *   this case, it will return the IP address of the network adaptor that
 *   provides the correct router to handle that destination address.
 *
 * @param  destipaddr - The destination IPv6 address
 * @param  srcipaddr  - The location to return that adaptor address that
 *   serves the sub-net that includes the destination address.
 *
 * @return
 *   Zero (OK) is returned on success with srcipaddr valid.  A negated
 *   errno value is returned on any failure and in this case the srcipaddr
 *   is not valid.
 */

int netlib_ipv6adaptor(FAR const struct in6_addr *destipaddr,
                       FAR struct in6_addr *srcipaddr);

/**
 * @brief
 *   Convert a 128-bit netmask to a prefix length.  The NuttX IPv6
 *   networking uses 128-bit network masks internally.  This function
 *   converts the IPv6 netmask to a prefix length.
 *
 *   The prefix length is the number of MS '1' bits on in the netmask.
 *   This, of course, assumes that all MS bits are '1' and all LS bits are
 *   '0' with no intermixed 1's and 0's.  This function searches from the
 *   MS bit until the first '0' is found (this does not necessary mean that
 *   there might not be additional '1' bits following the firs '0', but
 *   that will be a malformed netmask.
 *
 * @param  mask   Points to an IPv6 netmask in the form of uint16_t[8]
 *
 * @return
 *   The prefix length, range 0-128 on success;  This function will not
 *   fail.
 */

uint8_t netlib_ipv6netmask2prefix(FAR const uint16_t *mask);

/**
 * @brief
 *   Convert a IPv6 prefix length to a network mask.  The prefix length
 *   specifies the number of MS bits under mask (0-128)
 *
 * @param  preflen Determines the width of the netmask (in bits).
 *   Range 0-128
 * @param  netmask The location to return the netmask.
 *
 * @return
 *   None
 */

void netlib_prefix2ipv6netmask(uint8_t preflen,
                               FAR struct in6_addr *netmask);
#ifdef CONFIG_NETLINK_ROUTE
struct neighbor_entry_s;

/**
 * @brief
 *   Attempt to read the entire Neighbor table into a buffer.
 *
 * @param  nbtab    The location to store the copy of the Neighbor table
 * @param  nentries The size of the provided 'nbtab' in number of entries
 *   each of size sizeof(struct neighbor_entry_s)
 *
 * @return
 *   The number of Neighbor table entries read is returned on success; a
 *   negated errno value is returned on failure.
 */

ssize_t netlib_get_nbtable(FAR struct neighbor_entry_s *nbtab,
                           unsigned int nentries);
#endif
#endif /* CONFIG_NET_IPv6 */

#ifdef CONFIG_NETDEV_WIRELESS_IOCTL

/**
 * @brief
 *   Get the wireless access point ESSID
 *
 * @param  ifname The name of the interface to use
 * @param  essid  Location to store the returned ESSID, size must be
 *            IW_ESSID_MAX_SIZE + 1 or larger
 * @param  idlen  size of memory set asdie for the ESSID.
 *
 * @return
 *   0 on success; -1 on failure (errno may not be set)
 */

int netlib_getessid(FAR const char *ifname, FAR char *essid, size_t idlen);

/**
 * @brief
 *   Set the wireless access point ESSID
 *
 * @param  ifname The name of the interface to use
 * @param  essid  Wireless ESSD address to set, size must be less then or
 *   equal to IW_ESSID_MAX_SIZE + 1 (including the NUL string terminator).
 *
 * @return
 *   0 on success; -1 on failure (errno may not be set)
 */

int netlib_setessid(FAR const char *ifname, FAR const char *essid);
#endif

#ifdef CONFIG_NET_VLAN

/**
 * @brief
 *   Add a VLAN interface to an existing network device.
 *
 * @param  ifname The name of the existing network device
 * @param  vlanid The VLAN identifier to be added
 * @param  prio   The default VLAN priority (PCP)
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_add_vlan(FAR const char *ifname, int vlanid, int prio);

/**
 * @brief
 *   Remove a VLAN interface from an existing network device.
 *
 * @retval vlanif, The name of the VLAN network device
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_del_vlan(FAR const char *vlanif);
#endif

#ifdef CONFIG_NET_ARP
/* ARP Table Support */

/**
 * @brief
 *   Delete the hardware address mapping for the provided protocol address.
 *
 * @param  inaddr The IPv4 address to use in the query
 * @param  ifname The Network device name
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_del_arpmapping(FAR const struct sockaddr_in *inaddr,
                          FAR const char *ifname);

/**
 * @brief
 *   Get the current hardware address mapping for the provided protocol
 *   address from the ARP table.
 *
 * @param  inaddr  The IPv4 address to use in the query
 * @param  macaddr The location to return the mapped Ethernet MAC address
 * @param  ifname  The Network device name
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_get_arpmapping(FAR const struct sockaddr_in *inaddr,
                          FAR uint8_t *macaddr, FAR const char *ifname);

/**
 * @brief
 *   Add the current hardware address mapping for the provided protocol
 *   address to the ARP table.
 *
 * @param  inaddr  The IPv4 address to use in the mapping
 * @param  macaddr The Ethernet MAC address to use in the mapping
 * @param  ifname  The Network device name
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_set_arpmapping(FAR const struct sockaddr_in *inaddr,
                          FAR const uint8_t *macaddr,
                          FAR const char *ifname);
#ifdef CONFIG_NETLINK_ROUTE
struct arpreq;

/**
 * @brief
 *   Attempt to read the entire ARP table into a buffer.
 *
 * @param  arptab   The location to store the copy of the ARP table
 * @param  nentries The size of the provided 'arptab' in number of entries
 *   each of size sizeof(struct arpreq)
 *
 * @return
 *   The number of ARP table entries read is returned on success; a negated
 *   errno value is returned on failure.
 */

ssize_t netlib_get_arptable(FAR struct arpreq *arptab,
                            unsigned int nentries);
#endif
#endif

#ifdef HAVE_ROUTE_PROCFS
#  ifdef CONFIG_NET_IPv4
#    define netlib_open_ipv4route()        fopen(IPv4_ROUTE_PATH, "r")
#    define netlib_close_ipv4route(stream) fclose(stream)

/**
 * @brief
 *   Read the next entry from the IPv4 routing table.
 *
 * @param  fd    The open file descriptor to the procfs' IPv4 routing
 *   table.
 * @param  route The location to return that the next routing table
 *   entry.
 *
 * @return
 *   sizeof(struct netlib_ipv4_route_s) is returned on success.  Zero is
 *   returned if the end of file is encountered.  A negated errno value is
 *   returned on any failure.
 */

ssize_t netlib_read_ipv4route(FILE *stream,
                              FAR struct netlib_ipv4_route_s *route);

/**
 * @brief
 *   Given a destination address that is not on a local network, query the
 *   IPv4 routing table, and return the IPv4 address of the routing that
 *   will provide access to the correct sub-net.
 *
 * @param  destipaddr - The destination address to use in the look-up (in
 *   network byte order).
 * @param  router     - The location to return that the IP address of the
 *   router (in network byte order)
 *
 * @return
 *   Zero is returned on success; a negated errno value is returned on any
 *   failure.
 */

int netlib_ipv4router(FAR const struct in_addr *destipaddr,
                      FAR struct in_addr *router);
#  endif
#  ifdef CONFIG_NET_IPv6
#    define netlib_open_ipv6route()        fopen(IPv6_ROUTE_PATH, "r")
#    define netlib_close_ipv6route(stream) fclose(stream)

/**
 * @brief
 *   Read the next entry from the IPv6 routing table.
 *
 * @param  fd    The open file descriptor to the procfs' IPv6 routing
 *   table.
 * @param  route The location to return that the next routing table
 *   entry.
 *
 * @return
 *   sizeof(struct netlib_ipv6_route_s) is returned on success.  Zero is
 *   returned if the end of file is encountered.  A negated errno value is
 *   returned on any failure.
 */

ssize_t netlib_read_ipv6route(FILE *stream,
                              FAR struct netlib_ipv6_route_s *route);

/**
 * @brief
 *   Given a destination address that is no on a local network, query the
 *   IPv6 routing table, and return the IPv6 address of the routing that
 *   will provide access to the correct sub-net.
 *
 * @param  destipaddr - The destination address to use in the look-up (in
 *   network byte order).
 * @param  router     - The location to return that the IP address of the
 *   router (in network byte order)
 *
 * @return
 *   Zero is returned on success; a negated errno value is returned on any
 *   failure.
 */

int netlib_ipv6router(FAR const struct in6_addr *destipaddr,
                      FAR struct in6_addr *router);
#  endif
#endif

#if defined(CONFIG_NETLINK_ROUTE) && defined(CONFIG_NET_ROUTE)
struct rtentry;  /* Forward reference */

/**
 * @brief
 *   Return a snapshot of the routing table for the selected address family.
 *
 * @param  rtelist  The location to store the list of devices.
 * @param  nentries The size of the provided 'rtelist' in number of
 *   entries. The size of one entry is given by sizeof(struct rtentry);
 * @param  family   - Address family.  See AF_* definitions in
 *   sys/socket.h.
 *
 * @return
 *   The number of routing table entries read is returned on success; a
 *   negated errno value is returned on failure.
 */

ssize_t netlib_get_route(FAR struct rtentry *rtelist,
                         unsigned int nentries, sa_family_t family);
#endif

#if defined(CONFIG_NET_IPv4) && defined(CONFIG_NETUTILS_DHCPC)
/* DHCP */

/**
 * @brief
 *   Perform DHCP in an attempt to get the IP
 *   address of the specified network device.
 *
 * @param  ifname The name of the interface to use
 *
 * @return
 *   0 on success
 *   Negative integer on failure
 */

int netlib_obtain_ipv4addr(FAR const char *ifname);
#endif

#ifdef CONFIG_NET_ICMPv6_AUTOCONF
/* ICMPv6 Autoconfiguration */

/**
 * @brief
 *   Perform ICMPv6 auto-configuration in an attempt to get the IP address
 *   of the specified network device.
 *
 * @param  ifname The name of the interface to use
 *
 * @return
 *   0 on success; -1 on failure (with the errno variable set appropriately)
 */

int netlib_icmpv6_autoconfiguration(FAR const char *ifname);

/* DHCPv6 */

/**
 * @brief
 *   Perform ICMPv6 auto-configuration and DHCPv6 in an attempt to
 *   get the IP address of the specified network device.
 *
 * @param  ifname The name of the interface
 *
 * @return
 *   0 on success; negative integer on failure
 */

int netlib_obtain_ipv6addr(FAR const char *ifname);
#endif

#ifdef CONFIG_NET_IPTABLES
/* iptables interface support */

#  ifdef CONFIG_NET_IPv4

/**
 * @brief
 *   Read current config from kernel space.
 *
 * @param  table The table name to read from.
 *
 * @return
 *   The pointer to the config, or NULL if failed.
 *   Caller must free it after use.
 */

FAR struct ipt_replace *netlib_ipt_prepare(FAR const char *table);

/**
 * @brief
 *   Set config into kernel space.
 *
 * @param  repl The config to commit.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ipt_commit(FAR const struct ipt_replace *repl);

/**
 * @brief
 *   Flush all config in the table.
 *
 * @param  table The table name to flush.
 * @param  hook  The hook to flush, NF_INET_NUMHOOKS for all.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ipt_flush(FAR const char *table, enum nf_inet_hooks hook);

/**
 * @brief
 *   Set policy for the table.  It's a common operation, but may only take
 *   effect on filter-related tables.
 *
 * @param  table   The table name to set policy.
 * @param  hook    The hook to set policy.
 * @param  verdict The verdict to set.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ipt_policy(FAR const char *table, enum nf_inet_hooks hook,
                      int verdict);

/**
 * @brief
 *   Append an entry into config, will be put to as last config of the
 *   chain corresponding to hook.
 *
 * @param  repl  The config (to set into kernel later).
 * @param  entry The entry to append.
 * @param  hook  The hook of the entry.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ipt_append(FAR struct ipt_replace **repl,
                      FAR const struct ipt_entry *entry,
                      enum nf_inet_hooks hook);

/**
 * @brief
 *   Insert an entry into config, will be put to as first config of
 *   the chain corresponding to hook.
 *
 * @param  repl    The config (to set into kernel later).
 * @param  entry   The entry to insert.
 * @param  hook    The hook of the entry.
 * @param  rulenum The place to insert, 1 = first.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ipt_insert(FAR struct ipt_replace **repl,
                      FAR const struct ipt_entry *entry,
                      enum nf_inet_hooks hook, int rulenum);

/**
 * @brief
 *   Delete an entry from config.
 *
 * @param  repl    The config (to set into kernel later).
 * @param  entry   The entry to delete, choose either entry or rulenum.
 * @param  hook    The hook of the entry.
 * @param  rulenum The place to delete, 1 = first, set entry to NULL to
 *   use this.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ipt_delete(FAR struct ipt_replace *repl,
                      FAR const struct ipt_entry *entry,
                      enum nf_inet_hooks hook, int rulenum);

/**
 * @brief
 *   Fill inifname and outifname into entry.
 *
 * @param  entry     The entry to fill.
 * @param  inifname  The input device name, NULL for no change.
 * @param  outifname The output device name, NULL for no change.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ipt_fillifname(FAR struct ipt_entry *entry,
                          FAR const char *inifname,
                          FAR const char *outifname);
#    ifdef CONFIG_NET_NAT
FAR struct ipt_entry *netlib_ipt_masquerade_entry(FAR const char *ifname);
#    endif
#    ifdef CONFIG_NET_IPFILTER

/**
 * @brief
 *   Alloc an entry with filter target.
 *
 * @param  target      The target name to apply.
 * @param  verdict     The verdict to set, compatible with reject
 *   target's code
 * @param  match_proto The protocol match type in the entry, 0 for
 *   no match.
 *
 * @return
 *   The pointer to the entry, or NULL if failed.
 *   Caller must free it after use.
 */

FAR struct ipt_entry *netlib_ipt_filter_entry(FAR const char *target,
                                              int verdict,
                                              uint8_t match_proto);
#    endif
#  endif /* CONFIG_NET_IPv4 */
#  ifdef CONFIG_NET_IPv6

/**
 * @brief
 *   Read current config from kernel space.
 *
 * @param  table The table name to read from.
 *
 * @return
 *   The pointer to the config, or NULL if failed.
 *   Caller must free it after use.
 */

FAR struct ip6t_replace *netlib_ip6t_prepare(FAR const char *table);

/**
 * @brief
 *   Set config into kernel space.
 *
 * @param  repl The config to commit.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ip6t_commit(FAR const struct ip6t_replace *repl);

/**
 * @brief
 *   Flush all config in the table.
 *
 * @param  table The table name to flush.
 * @param  hook  The hook to flush, NF_INET_NUMHOOKS for all.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ip6t_flush(FAR const char *table, enum nf_inet_hooks hook);

/**
 * @brief
 *   Set policy for the table.  It's a common operation, but may only take
 *   effect on filter-related tables.
 *
 * @param  table   The table name to set policy.
 * @param  hook    The hook to set policy.
 * @param  verdict The verdict to set.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ip6t_policy(FAR const char *table, enum nf_inet_hooks hook,
                       int verdict);

/**
 * @brief
 *   Append an entry into config, will be put to as last config of the
 *   chain corresponding to hook.
 *
 * @param  repl  The config (to set into kernel later).
 * @param  entry The entry to append.
 * @param  hook  The hook of the entry.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ip6t_append(FAR struct ip6t_replace **repl,
                       FAR const struct ip6t_entry *entry,
                       enum nf_inet_hooks hook);

/**
 * @brief
 *   Insert an entry into config, will be put to as first config of the
 *   chain corresponding to hook.
 *
 * @param  repl    The config (to set into kernel later).
 * @param  entry   The entry to insert.
 * @param  hook    The hook of the entry.
 * @param  rulenum The place to insert, 1 = first.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ip6t_insert(FAR struct ip6t_replace **repl,
                       FAR const struct ip6t_entry *entry,
                       enum nf_inet_hooks hook, int rulenum);

/**
 * @brief
 *   Delete an entry from config.
 *
 * @param  repl    The config (to set into kernel later).
 * @param  entry   The entry to delete, choose either entry or rulenum.
 * @param  hook    The hook of the entry.
 * @param  rulenum The place to delete, 1 = first, set entry to NULL to
 *   use this.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ip6t_delete(FAR struct ip6t_replace *repl,
                       FAR const struct ip6t_entry *entry,
                       enum nf_inet_hooks hook, int rulenum);

/**
 * @brief
 *   Fill inifname and outifname into entry.
 *
 * @param  entry     The entry to fill.
 * @param  inifname  The input device name, NULL for no change.
 * @param  outifname The output device name, NULL for no change.
 *
 * @return
 *   0 on success; a negated errno value on failure.
 */

int netlib_ip6t_fillifname(FAR struct ip6t_entry *entry,
                           FAR const char *inifname,
                           FAR const char *outifname);
#    ifdef CONFIG_NET_IPFILTER

/**
 * @brief
 *   Alloc an entry with filter target.
 *
 * @param  target      The target name to apply.
 * @param  verdict     The verdict to set, compatible with reject
 *   target's code
 * @param  match_proto The protocol match type in the entry, 0 for
 *   no match.
 *
 * @return
 *   The pointer to the entry, or NULL if failed.
 *   Caller must free it after use.
 */

FAR struct ip6t_entry *netlib_ip6t_filter_entry(FAR const char *target,
                                                int verdict,
                                                uint8_t match_proto);
#    endif
#  endif /* CONFIG_NET_IPv6 */
#endif /* CONFIG_NET_IPTABLES */

#ifdef CONFIG_NETLINK_NETFILTER
/* Netfilter connection tracking support */

struct nlmsghdr;  /* Forward reference */

/**
 * @brief
 *   Parse a conntrack message.
 *
 * @param  nlh The netlink message to parse.
 * @param  ct  The conntrack to fill.
 *
 * @return
 *   OK on success, -errno on failure.
 */

int netlib_parse_conntrack(FAR const struct nlmsghdr *nlh, size_t len,
                           FAR struct netlib_conntrack_s *ct);

/**
 * @brief
 *   Get the conntrack table.
 *
 * @param  family The address family to get the conntrack table for.
 * @param  cb     The callback to call for each conntrack entry.
 *
 * @return
 *   The number of conntrack entries processed on success, -errno on
 *   failure.
 */

int netlib_get_conntrack(sa_family_t family, netlib_conntrack_cb_t cb);
#endif

/* HTTP support */

/**
 * @brief
 *   Parse a full HTTP URL string into its hostname, port, and filename
 *   components.
 *
 * @param url      The null-terminated HTTP URL string to parse.
 * @param port     A pointer to a uint16_t to store the parsed port number.
 * @param hostname A buffer to store the extracted hostname.
 * @param hostlen  The size of the 'hostname' buffer.
 * @param filename A buffer to store the extracted filename/path.
 * @param namelen  The size of the 'filename' buffer.
 *
 * @return
 *   OK on success.
 * @retval  EINVAL if the URL does not start with "http://".
 * @retval  ENOENT if the URL is missing a path component.
 * @retval  E2BIG if the 'hostname' or 'filename' buffer is too small.
 */

int netlib_parsehttpurl(FAR const char *url, uint16_t *port,
                        FAR char *hostname, int hostlen,
                        FAR char *filename, int namelen);

/**
 * @brief
 *   Parse an URL, not only HTTP ones. The parsing is according to this rule:
 *   SCHEME :// HOST [: PORT] / PATH
 *   - scheme is everything before the first colon
 *   - scheme must be followed by ://
 *   - host is everything until colon or slash
 *   - port is optional, parsed only if host ends with colon
 *   - path is everything after the host.
 *   This is noticeably simpler that the official URL parsing method, since
 *   - it does not take into account the user:pass@ part that can be present
 *     before the host. Support of these fields is planned in the url_s
 *     structure, but it is not parsed yet/
 *   - it does not separate the URL parameters nor the bookmark
 *   Note: see here for the documentation of a complete URL parsing routine:
 *   https://www.php.net/manual/fr/function.parse-url.php
 */

#ifdef CONFIG_NETUTILS_NETLIB_GENERICURLPARSER
int netlib_parseurl(FAR const char *str, FAR struct url_s *url);
#endif

/* Generic server logic */

/**
 * @brief
 *   Implement basic server listening
 *
 * @param  portno    The port to listen on (in network byte order)
 *
 * @return
 *   A valid listening socket or -1 on error.
 */

int netlib_listenon(uint16_t portno);

/**
 * @brief
 *   Implement basic server logic
 *
 * @param  portno    The port to listen on (in network byte order)
 * @param  handler   The entrypoint of the task to spawn when a new
 *   connection is accepted.
 * @param  stacksize The stack size needed by the spawned task
 *
 * @return
 *   Does not return unless an error occurs.
 */

void netlib_server(uint16_t portno, pthread_startroutine_t handler,
                   int stacksize);

/**
 * @brief
 *   Get the network driver ifup/ifdown status
 *
 * @param  ifname The name of the interface to use
 * @param  flags  The interface flags returned by SIOCGIFFLAGS
 *
 * @return
 *   0 on success; -1 on failure.
 */

int netlib_getifstatus(FAR const char *ifname, FAR uint8_t *flags);

/**
 * @brief
 *   Set the network interface UP
 *
 * @param  ifname The name of the interface to use
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_ifup(FAR const char *ifname);

/**
 * @brief
 *   Set the network interface DOWN
 *
 * @param  ifname The name of the interface to use
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_ifdown(FAR const char *ifname);

/**
 * @brief
 *   Enable ARP learning capability on the interface
 *
 * @param  ifname The name of the interface to use
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_ifarp(const char *ifname);

/**
 * @brief
 *   Disable ARP learning capability on the interface
 *
 * @param  ifname The name of the interface to use
 *
 * @return
 *   0 on success; -1 on failure
 */

int netlib_ifnoarp(const char *ifname);

/* DNS server addressing */

/**
 * @brief
 *   Set the DNS server IPv4 address
 *
 * @param  inaddr The address to set
 *
 * @return
 *   Zero (OK) is returned on success; A negated errno value is returned
 *   on failure.
 */

#if defined(CONFIG_NET_IPv4) && defined(CONFIG_NETDB_DNSCLIENT)
int netlib_set_ipv4dnsaddr(FAR const struct in_addr *inaddr);
#endif

/**
 * @brief
 *   Set the DNS server IPv6 address
 *
 * @param  inaddr The address to set
 *
 * @return
 *   Zero (OK) is returned on success; A negated errno value is returned
 *   on failure.
 */

#if defined(CONFIG_NET_IPv6) && defined(CONFIG_NETDB_DNSCLIENT)
int netlib_set_ipv6dnsaddr(FAR const struct in6_addr *inaddr);
#endif

/**
 * @brief
 *   Clear the DNS server address
 *
 * @param  void
 *
 * @return
 *  void
 */

#ifdef CONFIG_NETDB_DNSCLIENT
void netlib_clear_dnsaddr(void);
#endif

/**
 * @brief
 *   Set MTU of the NuttX driver state structure
 *
 * @param  ifname The name of the interface to use
 * @param  mtu    Maximum Transmission Unit，MTU
 *
 * @return:
 *   0 on success; -1 on failure (errno may not be set)
 */

int netlib_set_mtu(FAR const char *ifname, int mtu);

/**
 * @brief
 *   Read the DEV RX/TX statistics.
 *
 * @param  ifname The interface to read.
 * @param  stat   The pointer to return dev statistics.
 *
 * @return
 *   0 on success. A negated errno value is returned
 *   on any failure.
 */

#if defined(CONFIG_NETDEV_STATISTICS)
int netlib_getifstatistics(FAR const char *ifname,
                           FAR struct netdev_statistics_s *stat);
#endif

/* Network check support */

/**
 * @brief
 *   Check the network driver ip conflict
 *
 * @param  ifname The name of the interface to use
 *
 * @return
 *   0 on no conflict; a nagtive on failure; 1 on conflict
 */

#ifdef CONFIG_NET_ARP_ACD
int netlib_check_ifconflict(FAR const char *ifname);
#endif

#ifdef CONFIG_NETUTILS_PING

/**
 * @brief
 *   Get the network dns status
 *
 * @param  ip      The ipv4 address to check
 * @param  timeout The max timeout of each ping
 * @param  retry   The retry times of ping
 *
 * @return
 *   nums of remote reply of ping; a nagtive on failure
 */

int netlib_check_ipconnectivity(FAR const char *ip, int timeout, int retry);

/**
 * @brief
 *   Get the network dns status
 *
 * @param  ifname  The name of the interface to use
 * @param  timeout The timeout of ping
 * @param  retry   The retry times of ping
 *
 * @return
 *   nums of gateway reply of ping; a nagtive on failure.
 */

int netlib_check_ifconnectivity(FAR const char *ifname,
                                int timeout, int retry);
#else
#define netlib_check_ipconnectivity(i, t, r) 1
#define netlib_check_ifconnectivity(i, t, r) 1
#endif

/**
 * @brief
 *   Get the network iob info.
 *
 * @param  iob The pointer to return iobinfo.
 *
 * @return
 *   0 on success; a nagtive on failure.
 */

#ifdef CONFIG_MM_IOB
int netlib_get_iobinfo(FAR struct iob_stats_s *iob);
#endif

/**
 * @brief
 *   Check the http status code
 *
 * @param  host        The remote host to check
 * @param  getmsg      The http GET message
 * @param  port        The port of remote host
 * @param  expect_code The except http code of http code
 *
 * @return
 *   0 on success; a nagtive on failure.
 */

int netlib_check_httpconnectivity(FAR const char *host,
                                  FAR const char *getmsg,
                                  int port, int expect_code);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __APPS_INCLUDE_NETUTILS_NETLIB_H */
