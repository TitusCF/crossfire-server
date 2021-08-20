#!/bin/sh

( cd ../.. && sh autogen.sh --host=x86_64-w64-mingw32.static --prefix= "$@")
