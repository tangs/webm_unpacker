#include <iostream>
#include <fstream>

#include "xx_webm.h"
#include "webm_unpacker.h"

void print(const char* str) {
    std::cout << "log: " << str << std::endl;
}

int main() {
//    auto path = "./res/1.webm";
//    auto path = "/Users/tangs/Desktop/tmp/webm/mokey_vp9_60fps.webm";
//    auto outputPath = "/Users/tangs/Desktop/tmp/webm/output";
//    struct xx::Webm webm;
//    if (int r = webm.LoadFromWebmWithPath(path)) {
//        return r;
//    }
//    if (int r = webm.SaveToPngs(outputPath, "abc_")) {
//        return r;
//    }
//    int a = 3;

    auto path = "/Users/tangs/Desktop/tmp/webm/mokey_vp9_60fps.webm";
//    auto path = "/Users/tangs/Desktop/tmp/webm/10.webm";
    set_debug_log_cb(print);
    std::ifstream ifs(path,
                          std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();
    int length = pos;
    char *pChars = new char[length];
    ifs.seekg(0, std::ios::beg);
    ifs.read(pChars, length);
    ifs.close();

//    auto ptr = decode_webm_by_data((uint8_t*)pChars, length);
    auto ptr = create_webm_decoder((uint8_t*)pChars, length, true, 4);
    auto ptr1 = create_webm_decoder((uint8_t*)pChars, length, true, 1);
    assert(ptr);
    assert(ptr1);
    while (!is_load_finish(ptr)) std::this_thread::sleep_for(10ms);
    while (!is_load_finish(ptr1)) std::this_thread::sleep_for(10ms);

//    init_decoder(ptr);
    auto frameCount = frames_count(ptr);
    auto frameCount1 = frames_count(ptr1);
    for (auto i = 0; i < frameCount; ++i) {
        while (!is_frame_load_finish(ptr, i)) std::this_thread::sleep_for(10ms);
        while (!is_frame_load_finish(ptr1, i)) std::this_thread::sleep_for(10ms);
        auto size1 = get_frame_data_size(ptr, i);
        auto size2 = get_frame_data_size(ptr1, i);
        assert(size1 == size2);
        auto frameData1 = get_frame_data(ptr, i);
        auto frameData2 = get_frame_data(ptr1, i);
        assert(strncmp((char*)frameData1, (char*)frameData2, size1) == 0);
    }
    auto pngCount = png_count(ptr);
    auto pngCount1 = png_count(ptr);
    destroy_decoder(ptr);
    destroy_decoder(ptr1);
    delete[] pChars;

    return 0;
}
