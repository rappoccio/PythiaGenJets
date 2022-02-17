#!/usr/bin/env bash
docker run --rm -it -v /tmp/.X11-unix:/tmp/.X11-unix -v ${PWD}:/home/physicist/ -v /mnt:/mnt -e DISPLAY=$ip:0 -p $1:8888 --entrypoint /bin/bash $2
