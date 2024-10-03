#!/usr/bin/env bash

mkdir -p build_debug
cd build_debug
cmake -S ../ -B . -G "Unix Makefiles" -DDEBUG=1
make
mv debug__client ../
mv debug__server ../
cd ..