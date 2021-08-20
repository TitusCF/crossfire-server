#!/bin/sh

(
  cd ../../utils/cre &&
  x86_64-w64-mingw32.static-qmake-qt5 &&
  make -j4
)
