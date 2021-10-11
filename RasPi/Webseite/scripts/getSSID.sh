#!/bin/bash

# get ssid and cut off surroundings
# get line with ssid from file.conf - only first | remove first 10 chars | reverse | remove first char | reverse
grep "ssid" /etc/wpa_supplicant/wpa_supplicant.conf -m 1 | cut -c11- | rev | cut -c 2- | rev