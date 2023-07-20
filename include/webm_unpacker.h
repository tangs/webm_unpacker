#pragma once

#include <cstdlib>
#include <cstdint>

#ifdef _WIN32
//_declspec(dllexport)只在windows下使用
#define EXPORT_DLL  _declspec(dllexport)
#else
#define EXPORT_DLL
#endif


extern "C" {
    typedef void (*log_cb)(const char*);

    EXPORT_DLL void set_debug_log_cb(log_cb cb);
    EXPORT_DLL void* create_webm_decoder(uint8_t* data, int len, bool loadFrames, int loadFramesThreadCount);
    EXPORT_DLL bool is_load_finish(void* ptr);
    EXPORT_DLL bool is_frame_load_finish(void* ptr, int frame);
    EXPORT_DLL int load_err_code(void* ptr);
    EXPORT_DLL int init_decoder(void* ptr);
    EXPORT_DLL void destroy_decoder(void* ptr);
    EXPORT_DLL void decode_frame(void* ptr, int frame);
    EXPORT_DLL void* decode_webm(const char *webmPath);
    EXPORT_DLL void* decode_webm_by_data(uint8_t* data, int len);
    EXPORT_DLL void release_webm(void* ptr);
    EXPORT_DLL int png_count(void* ptr);
    EXPORT_DLL int frames_count(void* ptr);
    EXPORT_DLL int abi_version(void* ptr);
    EXPORT_DLL int get_webm_width(void* ptr);
    EXPORT_DLL int get_webm_height(void* ptr);
    EXPORT_DLL uint8_t* get_frame_data(void* ptr, int frame);
    EXPORT_DLL int get_frame_data_size(void* ptr, int frame);

    EXPORT_DLL int unpack_webm(const char *webmPath, const char *outPath, const char *prefix);
    EXPORT_DLL int unpack_webm1(uint8_t* data, int len, const char *outPath, const char *prefix);
    EXPORT_DLL int foo();
}