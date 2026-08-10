/****************************************************************************
 * apps/include/wireless/wapi.h
 *
 *   Copyright (C) 2017, 2019 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Adapted for NuttX from WAPI:
 *
 *   Copyright (c) 2010, Volkan YAZICI <volkan.yazici@gmail.com>
 *   All rights reserved.
 *
 * And includes WPA supplicant logic contributed by:
 *
 *   Author: Simon Piriou <spiriou31@gmail.com>
 *
 * Which was adapted to NuttX from driver_ext.h
 *
 *   Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __APPS_INCLUDE_WIRELESS_WAPI_H
#define __APPS_INCLUDE_WIRELESS_WAPI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <nuttx/wireless/wireless.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/**
 * @cond
 * Maximum allowed ESSID size.
 */

#define WAPI_ESSID_MAX_SIZE IW_ESSID_MAX_SIZE

/* Buffer size while reading lines from PROC_NET_ files. */

#define WAPI_PROC_LINE_SIZE  1024

#ifndef CONFIG_WIRELESS_WAPI_INITCONF
#  define wapi_load_config(ifname, confname, conf) NULL
#  define wapi_unload_config(load)
#  define wapi_save_config(ifname, confname, conf) 0
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Frequency flags. */

enum wapi_freq_flag_e
{
  WAPI_FREQ_AUTO  = IW_FREQ_AUTO,
  WAPI_FREQ_FIXED = IW_FREQ_FIXED
};

/* Route target types. */

enum wapi_route_target_e
{
  WAPI_ROUTE_TARGET_NET,              /* The target is a network. */
  WAPI_ROUTE_TARGET_HOST              /* The target is a host. */
};

/* ESSID flags.  */

enum wapi_essid_flag_e
{
  WAPI_ESSID_OFF = IW_ESSID_OFF,
  WAPI_ESSID_ON  = IW_ESSID_ON,

/* Extended flag "WAPI_ESSID_DELAY_ON" instructs the driver
 * to delay the connection behavior of essid, so that which can accept
 * more accurate information before generating a connection.
 *
 * About flow of wapi command, drivers that support WAPI_ESSID_DELAY_ON
 * semantics will have the following behavior changes:
 *
 * 1. Station mode without bssid set:
 *
 * $ ifup wlan0
 * $ wapi mode wlan0 2
 * $ wapi psk wlan0 12345678 3
 * $ wapi essid wlan0 archer 1
 * $ renew wlan0
 *
 * 2. Station mode with bssid set:
 *
 * $ ifup wlan0
 * $ wapi mode wlan0 2
 * $ wapi psk wlan0 12345678 3
 * $ wapi essid wlan0 archer 2  <-- WAPI_ESSID_DELAY_ON will indicate the
 * $                                driver delay the connection event late
 * $                                to bssid set (SIOCSIWAP).
 * $ wapi ap wlan0 ec:41:18:e0:76:7e
 * $ renew wlan0
 */

  WAPI_ESSID_DELAY_ON = IW_ESSID_DELAY_ON
};

/* Supported operation modes. */

enum wapi_mode_e
{
  WAPI_MODE_AUTO    = IW_MODE_AUTO,    /* Driver decides. */
  WAPI_MODE_ADHOC   = IW_MODE_ADHOC,   /* Single cell network. */
  WAPI_MODE_MANAGED = IW_MODE_INFRA,   /* Multi cell network, roaming, ... */
  WAPI_MODE_MASTER  = IW_MODE_MASTER,  /* Synchronisation master or access point. */
  WAPI_MODE_REPEAT  = IW_MODE_REPEAT,  /* Wireless repeater, forwarder. */
  WAPI_MODE_SECOND  = IW_MODE_SECOND,  /* Secondary master/repeater, backup. */
  WAPI_MODE_MONITOR = IW_MODE_MONITOR, /* Passive monitor, listen only. */
  WAPI_MODE_MESH    = IW_MODE_MESH     /* Mesh (IEEE 802.11s) network */
};

/* Flags for encoding */

enum wapi_encode_e
{
  WAPI_ENCODE_INDEX      = IW_ENCODE_INDEX,       /* Token index (if needed) */
  WAPI_ENCODE_FLAGS      = IW_ENCODE_FLAGS,       /* Flags defined below */
  WAPI_ENCODE_MODE       = IW_ENCODE_MODE,        /* Modes defined below */
  WAPI_ENCODE_DISABLED   = IW_ENCODE_DISABLED,    /* Encoding disabled */
  WAPI_ENCODE_ENABLED    = IW_ENCODE_ENABLED,     /* Encoding enabled */
  WAPI_ENCODE_RESTRICTED = IW_ENCODE_RESTRICTED,  /* Refuse non-encoded packets */
  WAPI_ENCODE_OPEN       = IW_ENCODE_OPEN,        /* Accept non-encoded packets */
  WAPI_ENCODE_NOKEY      = IW_ENCODE_NOKEY,       /* Key is write only, so not present */
  WAPI_ENCODE_TEMP       = IW_ENCODE_TEMP         /* Temporary key */
};

/* Bitrate flags.
 *
 * At the moment, unicast (IW_BITRATE_UNICAST) and broadcast
 * (IW_BITRATE_BROADCAST) bitrate flags are not supported.
 */

enum wapi_bitrate_flag_e
{
  WAPI_BITRATE_AUTO,
  WAPI_BITRATE_FIXED
};

/* Transmit power (txpower) flags. */

enum wapi_txpower_flag_e
{
  WAPI_TXPOWER_DBM,                   /* Value is in dBm. */
  WAPI_TXPOWER_MWATT,                 /* Value is in mW. */
  WAPI_TXPOWER_RELATIVE               /* Value is in arbitrary units. */
};

/* Linked list container for strings. */

struct wapi_string_s
{
  FAR struct wapi_string_s *next;
  FAR char *data;
};

/* Linked list container for scan results. */

struct wapi_scan_info_s
{
  FAR struct wapi_scan_info_s *next;
  struct ether_addr ap;
  int has_essid;
  char essid[WAPI_ESSID_MAX_SIZE + 1];
  enum wapi_essid_flag_e essid_flag;
  int has_freq;
  double freq;
  int has_mode;
  enum wapi_mode_e mode;
  int has_bitrate;
  int bitrate;
  int has_rssi;
  int rssi;
  int has_encode;
  int encode;
};

/* Linked list container for routing table rows. */

struct wapi_route_info_s
{
  FAR struct wapi_route_info_s *next;
  FAR char *ifname;
  struct in_addr dest;
  struct in_addr gw;

  unsigned int flags;  /* See  RTF_* in  net/route.h for available values. */
  unsigned int refcnt;
  unsigned int use;
  unsigned int metric;
  struct in_addr netmask;
  unsigned int mtu;
  unsigned int window;
  unsigned int irtt;
};

/* A generic linked list container. For functions taking  struct wapi_list_s
 * type of argument, caller is responsible for releasing allocated memory.
 */

struct wapi_list_s
{
  union wapi_list_head_t
  {
    FAR struct wapi_string_s     *string;
    FAR struct wapi_scan_info_s  *scan;
    FAR struct wapi_route_info_s *route;
  } head;
};

/* WPA **********************************************************************/

enum wpa_alg_e
{
  WPA_ALG_NONE = 0,
  WPA_ALG_WEP,
  WPA_ALG_TKIP,
  WPA_ALG_CCMP,
  WPA_ALG_IGTK,
  WPA_ALG_PMK,
  WPA_ALG_GCMP,
  WPA_ALG_SMS4,
  WPA_ALG_KRK,
  WPA_ALG_GCMP_256,
  WPA_ALG_CCMP_256,
  WPA_ALG_BIP_GMAC_128,
  WPA_ALG_BIP_GMAC_256,
  WPA_ALG_BIP_CMAC_256
};

enum wpa_ver_e
{
  WPA_VER_NONE = 0,
  WPA_VER_1,
  WPA_VER_2,
  WPA_VER_3
};

/* This structure provides the wireless configuration to
 * wpa_driver_wext_associate().
 */

struct wpa_wconfig_s
{
  enum wapi_mode_e sta_mode;     /* Mode of operation, e.g. IW_MODE_INFRA */
  uint8_t auth_wpa;              /* IW_AUTH_WPA_VERSION values, e.g.
                                  * IW_AUTH_WPA_VERSION_WPA2 */
  uint8_t cipher_mode;           /* IW_AUTH_PAIRWISE_CIPHER and
                                  * IW_AUTH_GROUP_CIPHER values, e.g.,
                                  * IW_AUTH_CIPHER_CCMP */
  enum wpa_alg_e alg;            /* See enum wpa_alg_e above, e.g.
                                  * WPA_ALG_CCMP */
  double freq;                   /* Channel frequency */
  enum wapi_freq_flag_e flag;    /* Channel frequency flag */
  uint8_t ssidlen;               /* Length of the SSID */
  uint8_t phraselen;             /* Length of the passphrase */
  FAR const char *ifname;        /* E.g., "wlan0" */
  FAR const char *ssid;          /* E.g., "myApSSID" */
  FAR const char *bssid;         /* Options to associate with bssid */
  FAR const char *passphrase;    /* E.g., "mySSIDpassphrase" */
};

/* COEX *********************************************************************/

enum wapi_pta_prio_e
{
  WAPI_PTA_PRIORITY_COEX_MAXIMIZED = IW_PTA_PRIORITY_COEX_MAXIMIZED,
  WAPI_PTA_PRIORITY_COEX_HIGH      = IW_PTA_PRIORITY_COEX_HIGH,
  WAPI_PTA_PRIORITY_BALANCED       = IW_PTA_PRIORITY_BALANCED,
  WAPI_PTA_PRIORITY_WLAN_HIGHD     = IW_PTA_PRIORITY_WLAN_HIGH,
  WAPI_PTA_PRIORITY_WLAN_MAXIMIZED = IW_PTA_PRIORITY_WLAN_MAXIMIZED
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/* Frequency flag names. */

EXTERN FAR const char *g_wapi_freq_flags[];

/* ESSID flag names. */

EXTERN FAR const char *g_wapi_essid_flags[];

/* Passphrase algorithm flag names. */

EXTERN FAR const char *g_wapi_alg_flags[];

/* Passphrase WPA Version flag names. */

EXTERN FAR const char *g_wapi_wpa_ver_flags[];

/* Supported operation mode names. */

EXTERN FAR const char *g_wapi_modes[];

/* Bitrate flag names. */

EXTERN FAR const char *g_wapi_bitrate_flags[];

/* Transmit power flag names. */

EXTERN FAR const char *g_wapi_txpower_flags[];

/* PTA priority flag names. */

EXTERN FAR const char *g_wapi_pta_prio_flags[];

/****************************************************************************
 * Public Function Prototyppes
 ****************************************************************************/

/**
 * @endcond
 * @brief
 *   Get the interface up status.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the network interface to query.
 * @param  is_up  Set to 0, if up; 1, otherwise.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_get_ifup(int sock, FAR const char *ifname, FAR int *is_up);

/**
 * @brief
 *   Activate the interface.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the network interface to query.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_ifup(int sock, FAR const char *ifname);

/**
 * @brief
 *   Shut down the interface.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the network interface to bring down.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_ifdown(int sock, FAR const char *ifname);

/**
 * @brief
 *   Get IP address of the given network interface.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the network interface to query.
 * @param  addr   An output pointer to be populated with the interface's
 *                IP address on success.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_get_ip(int sock, FAR const char *ifname, struct in_addr *addr);

/**
 * @brief
 *   Set IP address of the given network interface.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the network interface whose IP address is
 *                to be set.
 * @param  addr   A pointer to a struct containing the new IP address.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_ip(int sock, FAR const char *ifname,
                FAR const struct in_addr *addr);

/**
 * @brief
 *   Get netmask of the given network interface.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the network interface to query.
 * @param  addr   An output pointer to be populated with the interface's
 *                netmask on success.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_get_netmask(int sock, FAR const char *ifname,
                     FAR struct in_addr *addr);

/**
 * @brief
 *   Set netmask of the given network interface.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the network interface to query.
 * @param  addr   A pointer to a struct containing the new netmask.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_netmask(int sock, FAR const char *ifname,
                     FAR const struct in_addr *addr);

/**
 * @brief
 *   Add gateway for the given target network.
 *
 * @param  sock        A socket descriptor, required for the ioctl
 *                     operation.
 * @param  targettype  Specifies the type of the target.
 * @param  target      A pointer to the destination IP address.
 * @param  netmask     A pointer to the netmask for the destination target.
 * @param  gw          A pointer to the gateway (router) IP address for
 *                     this route.
 *
 * @return OK (0) on success; a negative errno value on failure.
 *
 * @note ifdef CONFIG_NET_ROUTE
 */

#ifdef CONFIG_NET_ROUTE
int wapi_add_route_gw(int sock, enum wapi_route_target_e targettype,
                      FAR const struct in_addr *target,
                      FAR const struct in_addr *netmask,
                      FAR const struct in_addr *gw);
#endif

/**
 * @brief
 *   Delete gateway for the given target network.
 *
 * @param  sock        A socket descriptor, required for the ioctl
 *                     operation.
 * @param  targettype  Specify the type of the target.
 * @param  target      A pointer to the destination IP address of the route
 *                     to be deleted.
 * @param  netmask     A pointer to the netmask for the destination target.
 * @param  gw          A pointer to the gateway (router) IP address for
 *                     this route.
 *
 * @return OK (0) on success; a negative errno value on failure.
 *
 * @note ifdef CONFIG_NET_ROUTE
 */

#ifdef CONFIG_NET_ROUTE
int wapi_del_route_gw(int sock, enum wapi_route_target_e targettype,
                      FAR const struct in_addr *target,
                      FAR const struct in_addr *netmask,
                      FAR const struct in_addr *gw);
#endif

/**
 * @brief
 *   Get the operating frequency of the device.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless network interface.
 * @param  freq   An output pointer to a double where the retrieved
 *                frequency (in Hz) will be stored.
 * @param  flag   An output pointer to an enum where the frequency mode
 *                will be stored.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_get_freq(int sock, FAR const char *ifname, FAR double *freq,
                  FAR enum wapi_freq_flag_e *flag);

/**
 * @brief
 *   Set the operating frequency of the device.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless network interface whose
 *                frequency is to be configured.
 * @param  freq   The desired operating frequency, specified as a double
 *                (in Hz).
 * @param  flag   The desired frequency mod.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_freq(int sock, FAR const char *ifname, double freq,
                  enum wapi_freq_flag_e flag);

/**
 * @brief
 *   Find corresponding channel for the supplied freq.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless interface.
 * @param  freq   The frequency, in Hz, to be converted to a channel number.
 * @param  chan   An output pointer to an integer.
 *
 * @return
 *   0, on success; -2, if not found; otherwise, ioctl() return value.
 */

int wapi_freq2chan(int sock, FAR const char *ifname, double freq,
                   FAR int *chan);

/**
 * @brief
 *   Find corresponding frequency for the supplied chan.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless interface whose channel map
 *                is to be used for the lookup.
 * @param  chan   The channel number to be converted to a frequency.
 * @param  freq   An output pointer to a double.
 *
 * @return
 *   0, on success; -2, if not found; otherwise, ioctl() return value.
 */

int wapi_chan2freq(int sock, FAR const char *ifname, int chan,
                   FAR double *freq);

/**
 * @brief
 *   Get ESSID of the device.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation
 * @param  ifname The name of the wireless network interface to query.
 * @param  essid  Used to store the ESSID of the device.
 * @param  flag   An output pointer to a variable where the ESSID status
 *                will be stored.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_get_essid(int sock, FAR const char *ifname, FAR char *essid,
                   FAR enum wapi_essid_flag_e *flag);

/**
 * @brief
 *    Set ESSID of the device.
 *
 *    essid At most WAPI_ESSID_MAX_SIZE characters are read.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless network interface to configure.
 * @param  essid  A pointer to a null-terminated string containing the ESSID
 *                to be set.
 * @param  flag   A flag to control the ESSID state.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_essid(int sock, FAR const char *ifname, FAR const char *essid,
                   enum wapi_essid_flag_e flag);

/**
 * @brief
 *   Get the operating mode of the device.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless interface to query.
 * @param  mode   An output pointer to a variable of type 'enum wapi_mode_e'
 *                where the resulting operating mode will be stored.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_get_mode(int sock, FAR const char *ifname,
                  FAR enum wapi_mode_e *mode);

/**
 * @brief
 *   Set the operating mode of the device.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless interface to query.
 * @param  mode   An output pointer to a variable of type 'enum wapi_mode_e'
 *                where the resulting operating mode will be stored.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_mode(int sock, FAR const char *ifname, enum wapi_mode_e mode);

/**
 * @brief
 *   Create an ethernet broadcast address.
 *
 * @param  sa  An output pointer to a `struct ether_addr` structure that
 *             will be filled with the broadcast address.
 *
 * @return Returns the result of the underlying wapi_make_ether() call,
 *         typically OK (0).
 */

int wapi_make_broad_ether(FAR struct ether_addr *sa);

/**
 * @brief
 *   Create an ethernet NULL address.
 *
 * @param  sa  An output pointer to a `struct ether_addr` structure that
 *             will be filled with the NULL address.
 *
 * @return Returns the result of the underlying wapi_make_ether() call,
 *         typically OK (0).
 */

int wapi_make_null_ether(FAR struct ether_addr *sa);

/**
 * @brief
 *   Get access point address of the device.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless interface to query.
 * @param  ap     Set the to MAC address of the device.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_get_ap(int sock, FAR const char *ifname, FAR struct ether_addr *ap);

/**
 * @brief
 *   Set access point address of the device.
 *
 * @param  sock   A socket descriptor, required for the ioctl operation.
 * @param  ifname The name of the wireless interface to configure.
 * @param  ap     The MAC address to set for the access point.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_ap(int sock, FAR const char *ifname,
                FAR const struct ether_addr *ap);

/**
 * @brief
 *   Get bitrate of the device.
 *
 * @param  sock    A socket descriptor, required for the ioctl operation.
 * @param  ifname  The name of the wireless interface to query.
 * @param  bitrate A pointer to a location to store the retrieved bitrate.
 * @param  flag    A pointer to a location to store the bitrate flag.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_get_bitrate(int sock, FAR const char *ifname,
                     FAR int *bitrate, FAR enum wapi_bitrate_flag_e *flag);

/**
 * @brief
 *   Set bitrate of the device.
 *
 * @param  sock    A socket descriptor, required for the ioctl operation.
 * @param  ifname  The name of the wireless interface to configure.
 * @param  bitrate The bitrate to set.
 * @param  flag    The bitrate flag.
 *
 * @return OK (0) on success; a negative errno value on failure.
 */

int wapi_set_bitrate(int sock, FAR const char *ifname, int bitrate,
                     enum wapi_bitrate_flag_e flag);

/**
 * @brief
 *   Convert a value in dBm to a value in milliWatt.
 *
 * @param  dbm The value in dBm to convert.
 *
 * @return The converted value in milliWatts.
 */

int wapi_dbm2mwatt(int dbm);

/**
 * @brief
 *   Convert a value in milliWatt to a value in dBm.
 *
 * @param  mwatt The value in milliWatt to convert.
 *
 * @return The converted value in dBm.
 */

int wapi_mwatt2dbm(int mwatt);

/**
 * @brief
 *   Get txpower of the device.
 *
 * @param  sock   The socket descriptor.
 * @param  ifname The interface name.
 * @param  power  A pointer to a variable to store the txpower value.
 * @param  flag   A pointer to a variable to store the unit of the txpower.
 *
 * @return 0 on success, a negated errno value on failure.
 */

int wapi_get_txpower(int sock, FAR const char *ifname, FAR int *power,
                     FAR enum wapi_txpower_flag_e *flag);

/**
 * @brief
 *   Set txpower of the device.
 *
 * @param  sock   The socket descriptor.
 * @param  ifname The interface name.
 * @param  power  The txpower value to set.
 * @param  flag   The unit of the txpower.
 *
 * @return 0 on success, a negated errno value on failure
 */

int wapi_set_txpower(int sock, FAR const char *ifname, int power,
                     enum wapi_txpower_flag_e flag);

/**
 * @brief
 *   Create an AF_INET socket to be used in ioctl() calls.
 *
 * @return
 *   Non-negative on success.
 */

int wapi_make_socket(void);

/**
 * @brief
 *   Start a scan on the given interface. Root privileges are required to
 *   start a scan.
 *
 * @param  sock   The socket descriptor.
 * @param  ifname The interface name.
 * @param  essid  The specific ESSID to scan for.
 *
 * @return 0 on success, a negated errno value on failure.
 */

int wapi_scan_init(int sock, FAR const char *ifname, FAR const char *essid);

/**
 * @brief
 *   Start a scan on the given interface. Root privileges are required to
 *   start a scan with specified channels.
 *
 * @param  sock         The socket descriptor.
 * @param  ifname       The interface name.
 * @param  essid        The specific ESSID to scan for.
 * @param  channels     Pointer to an array of channel numbers to scan.
 * @param  num_channels The number of channels in the array.
 *
 * @return 0 on success, a negated errno value on failure.
 */

int wapi_scan_channel_init(int sock, FAR const char *ifname,
                           FAR const char *essid,
                           uint8_t *channels, int num_channels);

/**
 * @brief
 *   Start a extended scan on the given interface, you can specify the
 *   scan type. Root privileges are required to start a scan.
 *
 * @param  sock      The socket descriptor.
 * @param  ifname    The interface name.
 * @param  scan_type The type of scan to perform.
 * @param  essid     The specific ESSID to scan for.
 *
 * @return 0 on success, a negated errno value on failure.
 */

int wapi_escan_init(int sock, FAR const char *ifname,
                    uint8_t scan_type, FAR const char *essid);

/**
 * @brief
 *   Start a scan on the given interface. Root privileges are required to
 *   start a scan with specified channels.
 *
 * @param  sock         The socket descriptor.
 * @param  ifname       The interface name.
 * @param  scan_type    The type of scan to perform.
 * @param  essid        The specific ESSID to scan for.
 * @param  channels     Pointer to an array of channel numbers to scan.
 * @param  num_channels The number of channels in the array.
 *
 * @return 0 on success, a negated errno value on failure.
 */

int wapi_escan_channel_init(int sock, FAR const char *ifname,
                            uint8_t scan_type, FAR const char *essid,
                            uint8_t *channels, int num_channels);

/**
 * @brief
 *   Check the status of the scan process.
 *
 * @param  sock   The socket descriptor.
 * @param  ifname The interface name.
 *
 * @return
 *   Zero, if data is ready; 1, if data is not ready; negative on failure.
 */

int wapi_scan_stat(int sock, FAR const char *ifname);

/**
 * @brief
 *   Collect the results of a scan process.
 *
 * @param  sock   The socket descriptor.
 * @param  ifname The interface name.
 * @param  aps    Pushes collected  struct wapi_scan_info_s into this list.
 *
 * @return 0 on successful collection and parsing, a negated errno value on
 *         failure.

 */

int wapi_scan_coll(int sock, FAR const char *ifname,
                   FAR struct wapi_list_s *aps);

/**
 * @brief
 *   Free the scan results.
 *
 * @param  aps   Release the collected struct wapi_scan_info_s.
 *
 * @return
 *   void
 */

void wapi_scan_coll_free(FAR struct wapi_list_s *aps);

/**
 * @brief
 *    Set the country code.
 *
 * @param  sock    The socket descriptor.
 * @param  ifname  The interface name.
 * @param  country A pointer to a two-character string representing the
 *                 country code.
 *
 * @return 0 on success, or a negated errno value on failure.
 */

int wapi_set_country(int sock, FAR const char *ifname,
                     FAR const char *country);

/**
 * @brief
 *    Get the country code.
 *
 * @param  sock    The socket descriptor.
 * @param  ifname  The interface name.
 * @param  country A pointer to a caller-provided buffer where the
 *                 two-character country code string will be stored.
 *
 * @return 0 on success, or a negated errno value on failure.
 */

int wapi_get_country(int sock, FAR const char *ifname,
                     FAR char *country);

/**
 * @brief
 *    Get the wlan Sensitivity.
 *
 * @param  sock   The socket descriptor.
 * @param  ifname The interface name.
 * @param  sense  A pointer to a caller-provided integer where the
 *                sensitivity value (in dBm) will be stored.
 *
 * @return 0 on success, or a negated errno value on failure.
 */

int wapi_get_sensitivity(int sock, FAR const char *ifname,
                         FAR int *sense);

#ifdef CONFIG_WIRELESS_WAPI_INITCONF
/**
 * @brief
 *   Load and parses wireless network configuration for a specific
 *   interface.
 *
 * @param ifname   The name of the wireless interface.
 * @param confname The path to the JSON configuration file. If NULL, a
 *                 default path is used.
 * @param conf     A pointer to a caller-provided structure to be filled
 *                 with the configuration data.
 *
 * @return
 *   Return a pointer to the hold the config resource, NULL On error.
 */

FAR void *wapi_load_config(FAR const char *ifname,
                           FAR const char *confname,
                           FAR struct wpa_wconfig_s *conf);

/**
 * @brief
 *   Release the resources allocated by wapi_load_config.
 *
 * @param load   Config resource handler, allocate by wapi_load_config().
 *
 * @return
 *   void.
 */

void wapi_unload_config(FAR void *load);

/**
 * @brief
 *   Save the wireless network configuration for a specified interface.
 *
 * @param  ifname    The name of the wireless interface for which the
 *                   configuration is to be saved.
 * @param  confname  The path to the JSON configuration file. If NULL, a
 *                   default path is used.
 * @param  conf      A pointer to a structure containing the configuration
 *                   data to be written to the file.
 *
 * @return 0 on success, or a negated errno value on failure.
 */

int wapi_save_config(FAR const char *ifname,
                     FAR const char *confname,
                     FAR const struct wpa_wconfig_s *conf);
#endif

/**
 * @brief
 *   Set an encryption key for a specific wireless interface.
 *
 * @param  sockfd   Opened network socket.
 * @param  ifname   Interface name.
 * @param  alg      The encryption algorithm to use.
 * @param  key      A pointer to the buffer containing the key material.
 * @param  key_len  The length of the key in bytes.
 *
 * @return 0 on success, or a negated errno value on failure.
 */

int wpa_driver_wext_set_key_ext(int sockfd, FAR const char *ifname,
                                enum wpa_alg_e alg, FAR const char *key,
                                size_t key_len);

/**
 * @brief
 *   Retrieve the currently configured encryption key and algorithm for a
 *   specific wireless interface.
 *
 * @param  sockfd   Opened network socket.
 * @param  ifname   Interface name.
 * @param  alg      A pointer to a variable where the retrieved encryption
 *                  algorithm will be stored.
 * @param  key      A pointer to a buffer where the key material will be
 *                  copied.
 * @param  req_len  An in-out parameter. On input, it must point to the size
 *                  of the 'key' buffer. On successful return, it will be
 *                  updated with the actual length of the retrieved key.
 *
 * @return 0 on success, or a negated errno value on failure.
 */

int wpa_driver_wext_get_key_ext(int sockfd, FAR const char *ifname,
                                enum wpa_alg_e *alg, FAR char *key,
                                size_t *req_len);

/**
 * @brief
 *   Configure and initiates an association for a wireless interface.
 *
 * @param  wconfig   Describes the wireless configuration.
 *
 * @return 0 on successful configuration, or a negated errno value on
 *         failure.
 */

int wpa_driver_wext_associate(FAR struct wpa_wconfig_s *wconfig);

/**
 * @brief
 *   Set a specific authentication parameter for a wireless interface.
 *
 * @param  sockfd   The socket file descriptor used for the underlying
 *                  ioctl call.
 * @param  ifname   The name of the wireless interface.
 * @param  idx      The index of the authentication parameter to set.
 * @param  value    The value to assign to the specified parameter.
 *
 * @return 0 on success, or a negated errno value on failure from the
 *         underlying ioctl call.
 */

int wpa_driver_wext_set_auth_param(int sockfd, FAR const char *ifname,
                                   int idx, uint32_t value);

/**
 * @brief
 *   Retrieve a specific authentication parameter from a wireless
 *   interface.
 *
 * @param  sockfd   The socket file descriptor used for the underlying
 *                  ioctl call.
 * @param  ifname   The name of the wireless interface.
 * @param  idx      The index of the authentication parameter to retrieve.
 * @param  value    A pointer to a variable where the retrieved parameter
 *                  value will be stored.
 *
 * @return 0 on success, or a negated errno value on failure from the
 *         underlying ioctl call.
 */

int wpa_driver_wext_get_auth_param(int sockfd, FAR const char *ifname,
                                   int idx, uint32_t *value);

/**
 * @brief
 *   Disconnect the specified wireless interface.
 *
 * @param  sockfd   The socket file descriptor used for the underlying
 *                  ioctl calls.
 * @param  ifname   The name of the wireless interface.
 *
 * @return
 *   void.
 */

void wpa_driver_wext_disconnect(int sockfd, FAR const char *ifname);

/**
 * @brief
 *   Set the pta priority of the device.
 *
 * @param  sock      The socket file descriptor used for the underlying
 *                   ioctl call.
 * @param  ifname    The name of the wireless interface.
 * @param  pta_prio  The PTA priority level to set, as defined by the
 *                   `enum wapi_pta_prio_e` enumeration.
 *
 * @return 0 on success, or a negated errno value on failure from the
 *         underlying ioctl call.
 */

int wapi_set_pta_prio(int sock, FAR const char *ifname,
                      enum wapi_pta_prio_e pta_prio);

/**
 * @brief
 *   Get the pta priority of the device.
 *
 * @param  sock      The socket file descriptor used for the underlying
 *                   ioctl call.
 * @param  ifname    The name of the wireless interface.
 * @param  pta_prio  A pointer to a variable where the current PTA priority
 *                   level will be stored upon successful retrieval.
 *
 * @return 0 on success, or a negated errno value on failure from the
 *         underlying ioctl call.
 */

int wapi_get_pta_prio(int sock, FAR const char *ifname,
                      enum wapi_pta_prio_e *pta_prio);

/**
 * @brief
 *   Set the wlan pmksa.
 *
 * @param  sock    The socket file descriptor used for the underlying
 *                 ioctl call.
 * @param  ifname  The name of the wireless interface.
 * @param  pmk     A pointer to the data buffer containing the PMKSA
 *                 information to be set. This typically includes the
 *                 BSSID of the AP and the PMK.
 * @param  len     The length of the data buffer pointed to by `pmk`.
 *
 * @return 0 on success, or a negated errno value on failure from the
 *         underlying ioctl call.
 */

int wapi_set_pmksa(int sock, FAR const char *ifname,
                   FAR const uint8_t *pmk, int len);

/**
 * @brief
 *   Get the wlan pmksa.
 *
 * @param  sock    The socket file descriptor used for the underlying
 *                 ioctl call.
 * @param  ifname  The name of the wireless interface.
 * @param  pmk     A pointer to a buffer where the retrieved PMKSA
 *                 information will be stored.
 * @param  len     The size of the buffer pointed to by `pmk`.
 *
 * @return 0 on success, or a negated errno value on failure from the
 *         underlying ioctl call.
 */

int wapi_get_pmksa(int sock, FAR const char *ifname,
                   FAR uint8_t *pmk, int len);

/**
 * @brief
 *   wapi extension interface for privatization method.
 *
 * @param  sock  The socket file descriptor used for the underlying
 *               ioctl call.
 * @param  cmd   The private ioctl command code to be executed. This
 *               command must be within the valid range for driver-specific
 *               operations.
 * @param  wrq   A pointer to an `iwreq` structure, which should be
 *               pre-populated by the caller with the interface name and
 *               any data required by the specific `cmd`.
 *
 * @return 0 on success. On failure, returns a negated errno value.
 */

int wapi_extend_params(int sock, int cmd, FAR struct iwreq *wrq);

/**
 * @brief
 *   Set power save status of wifi.
 *
 * @param  sock    The socket file descriptor used for the underlying
 *                 ioctl call.
 * @param  ifname  The name of the wireless interface.
 * @param  on      A boolean flag to control the power-saving state.
 *
 * @return 0 on success, or a negated errno value on failure.
 */

int wapi_set_power_save(int sock, FAR const char *ifname, bool on);

/**
 * @brief
 *   Get power save status of wifi.
 *
 * @param  sock    The socket file descriptor used for the underlying
 *                 ioctl call.
 * @param  ifname  The name of the wireless interface.
 * @param  on      A pointer to a boolean variable where the current
 *                 power-saving status will be stored.
 *
 * @return 0 on success, or a negated errno value on failure.
 */

int wapi_get_power_save(int sock, FAR const char *ifname, bool *on);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __APPS_INCLUDE_WIRELESS_WAPI_H */
