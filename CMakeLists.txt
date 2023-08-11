cmake_minimum_required(VERSION 3.21)
project(webm_unpacker)

include_directories(
        include
        libvpx/include
)

if (ANDROID_64)
    link_directories(
            libvpx/lib/android/arm64-v8a
    )
elseif (ANDROID_32)
    link_directories(
            libvpx/lib/android/armeabi-v7a
    )
elseif (IOS)
#    add_subdirectory(third/libvpx_prebuilt/gnusrc gnusrc)
elseif (WIN32)
#    windows 平台可能需要改下路径
    add_subdirectory(third/libvpx_prebuilt/gnusrc gnusrc)
else ()
    link_directories(
            libvpx/lib/osx
    )
endif ()

set(CMAKE_CXX_STANDARD 20)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (IOS)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
    include_directories(
            third/libvpx_prebuilt/gnusrc
    )

    set(SRCS)
    file(GLOB_RECURSE SRCS
            *.c
            *.h
            *.cpp
    )
#    message("src: ${SRCS}")
    get_filename_component(full_path_main_cpp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ABSOLUTE)
    list(REMOVE_ITEM SRCS "${full_path_main_cpp}")

    set(CMAKE_OSX_ARCHITECTURES "arm64")
    add_library(webm_unpacker STATIC
            ${SRCS}
    )
else ()
    add_library(webm_unpacker SHARED
            src/webm_unpacker.cpp
    )

    add_executable(webm_unpacker_test
            main.cpp
            src/webm_unpacker.cpp
    )
    target_link_libraries(webm_unpacker_test vpxgnu)
    target_link_libraries(webm_unpacker vpxgnu)
endif ()

