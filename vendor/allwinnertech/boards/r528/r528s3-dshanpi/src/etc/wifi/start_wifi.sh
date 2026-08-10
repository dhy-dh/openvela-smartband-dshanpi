#!/bin/sh
set +e

echo "[wifi] starting wlan0"
ifup wlan0
wapi country wlan0 CN
wapi mode wlan0 2

# The RTL8723/RTL871X firmware performs RF calibration immediately after
# ifup.  Starting WPA while that calibration is still running can leave the
# station repeating EAPOL message 2 until the AP deauthenticates it.
sleep 5
wapi reconnect wlan0
sleep 15

if renew wlan0
then
  echo "[wifi] connected and DHCP completed"
  ntpcstart &
else
  echo "[wifi] first attempt failed; restarting association"
  wapi disconnect wlan0
  sleep 3
  wapi mode wlan0 2
  wapi reconnect wlan0
  sleep 15
  if renew wlan0
  then
    echo "[wifi] retry connected and DHCP completed"
    ntpcstart &
  else
    echo "[wifi] retry failed"
  fi
fi

echo "[wifi] startup finished"
