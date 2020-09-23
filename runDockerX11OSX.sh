#!/usr/bin/env bash

ip=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
xhost + $ip
docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -v ${PWD}:/home/physicist/ -p $1:8888 $2
