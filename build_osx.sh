#!/usr/bin/env sh

mkdir -p build/osx
cd build/osx || exit 1

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    ../..

make
