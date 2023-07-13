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

//    auto ptr = decode_webm_by_data((u_int8_t*)pChars, length);
    auto ptr = create_webm_decoder((u_int8_t*)pChars, length, true, 4);
    assert(ptr);
    while (!is_load_finish(ptr)) std::this_thread::sleep_for(10ms);

//    init_decoder(ptr);
    auto frameCount = frames_count(ptr);
    for (auto i = 0; i < frameCount; ++i) {
        while (!is_frame_load_finish(ptr, i)) ;
//        decode_frame(ptr, i);
    }
    auto pngCount = png_count(ptr);
    destroy_decoder(ptr);
    delete[] pChars;

    return 0;
}
