#!/bin/sh
libtoolize -f -c
mv -f ltmain.sh utils
aclocal
autoheader
automake -a -c
autoconf
./configure $*

