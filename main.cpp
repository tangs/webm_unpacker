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


    set_debug_log_cb(print);
    std::ifstream ifs("/Users/tangs/Desktop/tmp/webm/mokey_vp9_60fps.webm",
                          std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();
    int length = pos;
    char *pChars = new char[length];
    ifs.seekg(0, std::ios::beg);
    ifs.read(pChars, length);
    ifs.close();

    auto ptr = decode_webm_by_data((u_int8_t*)pChars, length);

    delete[] pChars;

    return 0;
}
