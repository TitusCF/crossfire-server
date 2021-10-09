#!/bin/sh

CURRENT=`cd ../.. && basename \`pwd\``

./docker-setup.sh &&
./docker-run.sh /bin/bash -c "cd ~/project/$CURRENT/make_win32/mxe && ./build-all-docker.sh"
