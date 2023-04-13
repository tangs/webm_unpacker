cmake_minimum_required(VERSION 3.21)
project(webm_unpacker)

include_directories(
        include
        libvpx/include
)

if(ANDROID_64)
    link_directories(
            libvpx/lib/android/arm64-v8a
    )
elseif(ANDROID_32)
    link_directories(
            libvpx/lib/android/armeabi-v7a
    )
else()
    link_directories(
            libvpx/lib/osx
    )
endif ()

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

#add_executable(webm_unpacker
#        main.cpp
#        src/webm_unpacker.c)

add_library(webm_unpacker SHARED
        src/webm_unpacker.cpp)
target_link_libraries(webm_unpacker vpxgnu)
