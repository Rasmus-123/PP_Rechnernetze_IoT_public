#!/bin/bash

while getopts s:p: flag
do
    case "${flag}" in
        s) ssid=${OPTARG};;
        p) psk=${OPTARG};;
    esac
done


echo "SSID: $ssid";
echo "PSK: $psk";

echo "country=DE
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
    ssid=\"${ssid}\"
    psk=\"${psk}\"
}

network={
    ssid=\"iot-test\"
    psk=\"raspberrypi42\"
}" > /etc/wpa_supplicant/wpa_supplicant.conf
