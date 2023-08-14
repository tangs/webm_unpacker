#!/usr/bin/env sh

rm -rf build/android/x86_64
mkdir -p build/android/x86_64
cd build/android/x86_64 || exit 1

cmake \
    -DCMAKE_TOOLCHAIN_FILE=/Users/tangs/Library/Android/sdk/ndk/23.1.7779620/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=x86_64 \
    -DANDROID_PLATFORM=android-23 \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_64=true \
    ../../..

make webm_unpacker
