#!/usr/bin/env sh

rm -rf build/ios
mkdir -p build/ios
cd build/ios || exit 1

cmake \
    -DCMAKE_TOOLCHAIN_FILE=./cmake/ios.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DIOS=true \
    -DPLATFORM=OS64 \
    -GXcode ../..
#cd ../..
cmake --build . --config Release

#make
