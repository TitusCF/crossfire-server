#!/bin/sh
libtoolize -f -c
aclocal -I macros --install
autoheader
automake -a -c
autoconf
./configure $*
