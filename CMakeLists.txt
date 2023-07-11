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
elseif(IOS)
    link_directories(
            libvpx/lib/ios
    )
else()
    link_directories(
            libvpx/lib/osx
    )
endif ()

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")


if (IOS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    add_library(webm_unpacker STATIC
            src/webm_unpacker.cpp
            )
else ()
    add_library(webm_unpacker SHARED
            src/webm_unpacker.cpp)
#    add_executable(webm_unpacker
#            main.cpp
#            src/webm_unpacker.cpp)
endif ()
target_link_libraries(webm_unpacker vpxgnu)
