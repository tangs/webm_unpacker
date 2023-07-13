#pragma once

#include <cstdlib>

extern "C" {
    typedef void (*log_cb)(const char*);

    void set_debug_log_cb(log_cb cb);
    void* create_webm_decoder(u_int8_t* data, int len);
    bool is_load_finish(void* ptr);
    int load_err_code(void* ptr);
    int init_decoder(void* ptr);
    void destroy_decoder(void* ptr);
    void decode_frame(void* ptr, int frame);
    void* decode_webm(const char *webmPath);
    void* decode_webm_by_data(u_int8_t* data, int len);
    void release_webm(void* ptr);
    int png_count(void* ptr);
    int frames_count(void* ptr);
    int abi_version(void* ptr);
    int get_webm_width(void* ptr);
    int get_webm_height(void* ptr);
    u_int8_t* get_frame_data(void* ptr, int frame);
    int get_frame_data_size(void* ptr, int frame);

    int unpack_webm(const char *webmPath, const char *outPath, const char *prefix);
    int unpack_webm1(u_int8_t* data, int len, const char *outPath, const char *prefix);
    int foo();
}