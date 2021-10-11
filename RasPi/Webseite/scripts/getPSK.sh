#!/bin/bash

# get ssid and cut off surroundings
# get line with ssid from file.conf - only first | remove first 9 chars | reverse | remove first char | reverse
grep "psk" /etc/wpa_supplicant/wpa_supplicant.conf -m 1 | cut -c10- | rev | cut -c 2- | rev