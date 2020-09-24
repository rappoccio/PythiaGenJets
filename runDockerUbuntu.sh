#!/usr/bin/env bash

ip=$(ifconfig docker0 | grep inet | awk '$1=="inet" {print $2}')
xhost + $ip
docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -v ${PWD}:/home/physicist/ -e DISPLAY=$ip:0 -p $1:8888 --entrypoint /bin/bash $2
