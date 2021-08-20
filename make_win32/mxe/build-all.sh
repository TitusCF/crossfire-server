#!/bin/sh

./docker-setup.sh &&
./docker-run.sh /bin/bash -c "cd ~/project/server/make_win32/mxe && ./build-all-docker.sh"
