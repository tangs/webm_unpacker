#include "webm_unpacker.h"

#include "xx_webm.h"
#include "svpng.inc"

struct WebmInfo {
    struct xx::Webm webm;
    std::vector<std::vector<u_int8_t>> pngs;
};

static void* print_cb;

void set_debug_log_cb(log_cb cb) {
    print_cb = (void*)cb;
}

void print_log(const std::string& log)
{
    if (print_cb == nullptr) return;
    ((log_cb)print_cb)(log.c_str());
}

void* decode_webm(const char *webmPath) {
    auto webmInfo = new WebmInfo();
    auto& webm = webmInfo->webm;
    int r = webm.LoadFromWebmWithPath(webmPath);
    if (r) {
        return nullptr;
    }
    webm.ForeachFrame([&](std::vector<uint8_t> const& bytes)->int {
        xx::Data d;
        svpng(d, webm.width, webm.height, bytes.data(), 1);
        std::vector<u_int8_t> png;
        png.resize(d.len);
        memcpy(png.data(), d.buf, d.len);
        webmInfo->pngs.push_back(std::move(png));
        return 0;
    });
    return (void*)webmInfo;
}

void* decode_webm_by_data(u_int8_t* data, int len) {
    auto result = std::make_unique<std::uint8_t[]>(len);
    memcpy(result.get(), data, len);

    auto webmInfo = new WebmInfo();
    auto& webm = webmInfo->webm;
    int r = webm.LoadFromWebm(std::move(result), len);
    if (r) {
        return nullptr;
    }
    webm.ForeachFrame([&](std::vector<uint8_t> const& bytes)->int {
        xx::Data d;
        svpng(d, webm.width, webm.height, bytes.data(), 1);
        std::vector<u_int8_t> png;
        png.resize(d.len);
        memcpy(png.data(), d.buf, d.len);
        webmInfo->pngs.push_back(std::move(png));
        return 0;
    });
    return (void*)webmInfo;
}


void release_webm(void* ptr) {
    if (ptr != nullptr) delete (WebmInfo*)ptr;
}

int frames_count(void* ptr) {
    auto info = (WebmInfo*)ptr;
    return (int)info->pngs.size();
}

int get_webm_width(void* ptr) {
    auto info = (WebmInfo*)ptr;
    return info->webm.width;
}

int get_webm_height(void* ptr){
    auto info = (WebmInfo*)ptr;
    return info->webm.height;
}

u_int8_t* get_frame_data(void* ptr, int frame) {
    auto info = (WebmInfo*)ptr;
    return info->pngs[frame].data();
}

int get_frame_data_size(void* ptr, int frame)
{
    auto info = (WebmInfo*)ptr;
    return (int)info->pngs[frame].size();
}



int unpack_webm(const char* webmPath, const char* outPath, const char* prefix) {
    struct xx::Webm webm;
    int r = webm.LoadFromWebmWithPath(webmPath);
    if (r) {
        return r;
    }
    r = webm.SaveToPngs(outPath, prefix);
    if (r) {
        return r;
    }
    return 0;
}


int unpack_webm1(u_int8_t *data, int size, const char *outPath, const char *prefix) {
    auto result = std::make_unique<std::uint8_t[]>(size);
    memcpy(result.get(), data, size);

    struct xx::Webm webm;
    int r = webm.LoadFromWebm(std::move(result), size);
    if (r) {
        return r;
    }
    r = webm.SaveToPngs(outPath, prefix);
    if (r) {
        return r;
    }
    return 0;
}

int foo() {
    return 100;
}

