#!/bin/sh
set +e

echo "[wifi] starting wlan0"
ifup wlan0
wapi country wlan0 CN
wapi mode wlan0 2

# Let the Realtek RF calibration complete before starting WPA.
sleep 5
wapi reconnect wlan0
sleep 15

if renew wlan0
then
  echo "[wifi] connected and DHCP completed"
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
  else
    echo "[wifi] retry failed"
  fi
fi

echo "[wifi] startup finished"
