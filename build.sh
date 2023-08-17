#!/bin/sh
mkdir -p build
cmake -B build -S "$PWD/Sources"
cd build || exit
make -j8
rm -rf Sources