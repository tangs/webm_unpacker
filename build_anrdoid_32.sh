#!/usr/bin/env sh

rm -rf build/android/armeabi-v7a
mkdir -p build/android/armeabi-v7a
cd build/android/armeabi-v7a || exit 1

cmake \
    -DCMAKE_TOOLCHAIN_FILE=/Users/tangs/Library/Android/sdk/ndk/23.1.7779620/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=armeabi-v7a \
    -DANDROID_PLATFORM=android-23 \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_32=true \
    ../../..

make
