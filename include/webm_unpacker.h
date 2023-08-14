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

    // loadFramesThreadCount: 为0时在当前线程解码.
    // convertTo16BitTexture: 转成RGBA4444, 目前Unity在用, cocos待测试.
    // skipFramesPerTimes: 为解1帧丢弃的帧数, 为0时不丢弃.
    EXPORT_DLL void* create_webm_decoder(uint8_t* data, int len, bool loadFrames,
                                         int loadFramesThreadCount, bool flipY,
                                         bool convertTo16BitTexture = false,
                                         int skipFramesPerTimes = 0);
    EXPORT_DLL void destroy_decoder(void* context);
    EXPORT_DLL bool is_load_finish(void* context);
    EXPORT_DLL bool is_frame_load_finish(void* context, int frame);
    EXPORT_DLL int load_err_code(void* context);
    EXPORT_DLL int init_decoder(void* context);
    EXPORT_DLL void decode_frame(void* context, int frame);
    EXPORT_DLL void* decode_webm(const char *webmPath);
    EXPORT_DLL void release_webm(void* context);
    EXPORT_DLL int frames_count(void* context);
    EXPORT_DLL int get_webm_width(void* context);
    EXPORT_DLL int get_webm_height(void* context);
    EXPORT_DLL uint8_t* get_frame_data(void* context, int frame);
    EXPORT_DLL int get_frame_data_size(void* context, int frame);
    EXPORT_DLL bool has_alpha_channel(void* context);
}