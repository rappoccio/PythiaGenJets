#!/bin/env bash

docker run -t -i -v ${PWD}:/home/physicist/zjets_example -p 8888:8888 srappoccio/fastjet-tutorial-root:latest
