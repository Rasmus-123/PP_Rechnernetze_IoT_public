#!/bin/bash

timestamp=$(date +%Y-%m-%d_%H-%M-%S)

file_name=Webserver_$timestamp.log

cd ~/Webserver/

nohup ./Webserver > ~/logs/$file_name 2>&1 &
