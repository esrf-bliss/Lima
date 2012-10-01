#!/bin/sh

if [[ ! -d build.release ]]; then
   mkdir build.release
fi

cd build.release

cmake ../src -DCMAKE_BUILD_TYPE=Release

make
