#!/bin/sh

if [[ ! -d build.debug ]]; then
   mkdir build.debug
fi

cd build.debug

cmake ../src -DCMAKE_BUILD_TYPE=Debug

make
