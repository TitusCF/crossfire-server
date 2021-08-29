#!/bin/sh

./configure-cross-compile-win.sh &&
( cd ../.. && make -j6 ) &&
./build-cre.sh
