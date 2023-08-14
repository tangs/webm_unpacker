#!/usr/bin/env sh

rm -rf build/osx
mkdir -p build/osx
cd build/osx || exit 1

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    ../..

make webm_unpacker
