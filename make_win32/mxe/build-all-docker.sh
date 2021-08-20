#!/bin/sh

./configure-cross-compile-win.sh &&
( cd ../.. && make -j ) &&
./build-cre.sh
