#!/usr/bin/env sh

mkdir -p build/android/arm64-v8a
cd build/android/arm64-v8a || exit 1

cmake \
    -DCMAKE_TOOLCHAIN_FILE=/Users/tangs/Library/Android/sdk/ndk/23.1.7779620/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-23 \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_64=true \
    ../../..

make
