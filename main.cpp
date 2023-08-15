#include <iostream>
#include <fstream>

#include "xx_webm.h"
#include "webm_unpacker.h"

void print(const char* str) {
    std::cout << "log: " << str << std::endl;
}

bool save_rgba_to_bmp(const std::string& path, const std::vector<unsigned char>& rgba, int width, int height) {
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

static std::vector<uint8_t> get_raw_frame_data(void* context, int frame) {
    auto frameData = get_frame_data(context, frame);
    auto size = get_frame_data_size(context, frame);
    std::vector<uint8_t> data;
    data.resize(size);
    memcpy(data.data(), frameData, size);
    return data;
}

void Test1(const char* pChars, int length) {
    auto ptr = create_webm_decoder((uint8_t*)pChars, length,
                                   true, 0, true);
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
//        assert(strncmp((char*)frameData1, (char*)frameData2, size1) == 0);
        auto bmpPath = "/Users/tangs/Desktop/tmp/bmp/" + std::to_string(i) + ".bmp";
        auto rgba = get_raw_frame_data(ptr, i);
        save_rgba_to_bmp(bmpPath,
                         rgba,
                         get_webm_width(ptr),
                         get_webm_height(ptr));
    }
    destroy_decoder(ptr);
    destroy_decoder(ptr1);
    release_webm(ptr);
    release_webm(ptr1);
}

void Test2(const char* pChars, int length, const std::string& outPath) {
    auto context = create_webm_decoder((uint8_t*)pChars, length,
                                   true, 0, false, false);
    assert(context);

    auto frameCount = frames_count(context);
    auto with = get_webm_width(context);
    auto height = get_webm_height(context);
    for (auto i = 0; i < frameCount; ++i) {
        auto size = get_frame_data_size(context, i);
        auto frameData1 = get_frame_data(context, i);
        auto path = outPath + std::to_string(i) + ".bmp";
        auto rgba = get_raw_frame_data(context, i);
        save_rgba_to_bmp(path, rgba, with, height);
    }

    destroy_decoder(context);
    release_webm(context);
}

void Test3(const char* pChars, int length) {
    auto ptr = create_webm_decoder((uint8_t*)pChars, length,
                                   true, 1, true, false, 3);
    assert(ptr);

    while (!is_load_finish(ptr)) std::this_thread::sleep_for(10ms);

    auto width = get_webm_width(ptr);
    auto height = get_webm_height(ptr);
//    init_decoder(ptr);
    auto frameCount = frames_count(ptr);
    for (auto i = 0; i < frameCount; ++i) {
        while (!is_frame_load_finish(ptr, i)) std::this_thread::sleep_for(10ms);
        auto size1 = get_frame_data_size(ptr, i);
        if (size1 == 0) continue;
        auto frameData1 = get_frame_data(ptr, i);
//        assert(strncmp((char*)frameData1, (char*)frameData2, size1) == 0);
        auto bmpPath = "/Users/tangs/Desktop/tmp/bmp/" + std::to_string(i) + ".bmp";
        auto rgba = get_raw_frame_data(ptr, i);
        save_rgba_to_bmp(bmpPath,
                         rgba,
                         width,
                         height);
    }
    destroy_decoder(ptr);
    release_webm(ptr);
}

int main() {
    auto path = "/Users/tangs/Desktop/tmp/webm/EN_UHDL_Bn_Ftr_In_00000.webm.txt";
//    auto path = "/Users/tangs/Desktop/未命名文件夹 2/webm/output.webm";
//    auto path = "/Users/tangs/Desktop/未命名文件夹 2/webm/335/vp9-4M/coin_new__00000.webm";
    auto outPath = "/Users/tangs/Desktop/tmp/bmp/";

    set_debug_log_cb(print);
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();
    auto length = (int)pos;
    char *pChars = new char[length];
    ifs.seekg(0, std::ios::beg);
    ifs.read(pChars, length);
    ifs.close();

    Test3(pChars, length);
//    Test2(pChars, length, outPath);

    delete[] pChars;

    return 0;
}
