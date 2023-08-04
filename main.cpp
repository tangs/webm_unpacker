#include <iostream>
#include <fstream>

#include "xx_webm.h"
#include "webm_unpacker.h"

void print(const char* str) {
    std::cout << "log: " << str << std::endl;
}

bool save_rgba_to_bmp(const std::string& path, const std::vector<unsigned char>& rgba, int width, int height)
{
    std::ofstream file(path.c_str(), std::ios::binary);
    if(!file) return false;

    // BMP 文件头
    unsigned char bmp_file_header[14] = {'B', 'M',
                                         0,   0, 0, 0,
                                         0,   0,
                                         0,   0,
                                         54,  0, 0, 0};

    // BMP 信息头
    unsigned char bmp_info_header[40] = {40,  0,  0,  0,
                                         0,   0,  0,  0,
                                         0,   0,  0,  0,
                                         1,   0,
                                         32,  0,
                                         0,   0,  0,  0,
                                         0,   0,  0,  0,
                                         0,   0,  0,  0,
                                         0,   0,  0,  0,
                                         0,   0,  0,  0,
                                         0,   0,  0,  0};

    size_t size = width * height * 4;
    unsigned int size_data = static_cast<unsigned int>(size);
    unsigned int size_all = size_data + sizeof(bmp_file_header) + sizeof(bmp_info_header);

    bmp_file_header[2] = (unsigned char)(size_all);
    bmp_file_header[3] = (unsigned char)(size_all >> 8);
    bmp_file_header[4] = (unsigned char)(size_all >> 16);
    bmp_file_header[5] = (unsigned char)(size_all >> 24);

    bmp_info_header[4] = (unsigned char)(width);
    bmp_info_header[5] = (unsigned char)(width >> 8);
    bmp_info_header[6] = (unsigned char)(width >> 16);
    bmp_info_header[7] = (unsigned char)(width >> 24);

    bmp_info_header[8] = (unsigned char)(height);
    bmp_info_header[9] = (unsigned char)(height >> 8);
    bmp_info_header[10] = (unsigned char)(height >> 16);
    bmp_info_header[11] = (unsigned char)(height >> 24);

    bmp_info_header[20] = (unsigned char)(size_data);
    bmp_info_header[21] = (unsigned char)(size_data >> 8);
    bmp_info_header[22] = (unsigned char)(size_data >> 16);
    bmp_info_header[23] = (unsigned char)(size_data >> 24);

    // 写入BMP文件和信息头
    file.write(reinterpret_cast<const char*>(bmp_file_header), sizeof(bmp_file_header));
    file.write(reinterpret_cast<const char*>(bmp_info_header), sizeof(bmp_info_header));

    size_t row_size = width * 4;
    auto row_ptr = &rgba[row_size * (height - 1)];
    for (int y = 0; y < height; ++y)
    {
        file.write(reinterpret_cast<const char*>(&*row_ptr), row_size);
        row_ptr -= row_size;
    }
    return true;
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

//    auto path = "/Users/tangs/Desktop/tmp/webm/mokey_vp9_60fps.webm";
    auto path = "/Users/tangs/Desktop/tmp/EN_UHDL_Bn_Ftr_In_00000.webm";
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
    auto ptr = create_webm_decoder((uint8_t*)pChars, length,
                                   true, 4, true);
    auto ptr1 = create_webm_decoder((uint8_t*)pChars, length,
                                    true, 1, false);
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
//        auto bmpPath = "/Users/tangs/Desktop/tmp/bmp/" + std::to_string(i) + ".bmp";
//        save_rgba_to_bmp(bmpPath,
//                         get_raw_frame_data(ptr, 1),
//                         get_webm_width(ptr),
//                         get_webm_height(ptr));
    }
    auto pngCount = png_count(ptr);
    auto pngCount1 = png_count(ptr);
    destroy_decoder(ptr);
    destroy_decoder(ptr1);
    delete[] pChars;

    return 0;
}
