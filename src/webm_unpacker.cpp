#include "webm_unpacker.h"

#include "xx_webm.h"
#include "svpng.inc"

struct WebmInfo {
    struct xx::Webm webm;
    std::vector<std::vector<u_int8_t>> pngs;
    vpx_codec_iface_t* iface = nullptr;
    vpx_codec_dec_cfg_t cfg;
    vpx_codec_ctx_t ctx;
    vpx_codec_ctx_t ctxAlpha;
    uint8_t const* rgbBuf = nullptr;
    uint8_t const* aBuf = nullptr;
    uint32_t rgbBufLen = 0;
    uint32_t aBufLen = 0;
};

static void* print_cb;

void set_debug_log_cb(log_cb cb) {
    print_cb = (void*)cb;
}

void print_log(const std::string& log)
{
    if (print_cb == nullptr)
    {
        std::cout << "webm log:" << log << std::endl;
        return;
    }
    ((log_cb)print_cb)(log.c_str());
}

void init_decoder(void* ptr)
{
    auto info = (WebmInfo*)ptr;
    auto& webm = info->webm;

    assert(webm.codecId);
    info->cfg = {1, webm.width, webm.height};
    info->iface = vpx_codec_vp9_dx();
    if (int r = vpx_codec_dec_init(&info->ctx, info->iface, &info->cfg, 0)) assert(false);
//    if (int r = vpx_codec_decode(&info->ctx, info->rgbBuf, info->rgbBufLen, nullptr, 0)) assert(false);
    if (webm.hasAlpha) {
        if (int r = vpx_codec_dec_init(&info->ctxAlpha, info->iface, &info->cfg, 0)) assert(false);
//        if (int r = vpx_codec_decode(&info->ctxAlpha, info->aBuf, info->aBufLen, nullptr, 0)) assert(false);
    }
}

void destroy_decoder(void* ptr)
{
    auto info = (WebmInfo*)ptr;
    auto& webm = info->webm;

    vpx_codec_destroy(&info->ctx);
    if (webm.hasAlpha) {
        vpx_codec_destroy(&info->ctxAlpha);
    }
}

void* create_webm_decoder(u_int8_t* data, int len) {
    auto result = std::make_unique<std::uint8_t[]>(len);
    memcpy(result.get(), data, len);

    auto webmInfo = new WebmInfo();
    auto& webm = webmInfo->webm;
    int r = webm.LoadFromWebm(std::move(result), len);
    if (r) {
        std::stringstream ss;
        ss << "load from webm fail: " << r;
        print_log(ss.str());
        return nullptr;
    }

    return (void*)webmInfo;
}

void decode_frame(void* ptr, int frame) {
    auto webmInfo = (WebmInfo*)ptr;
    auto& webm = webmInfo->webm;
    auto bytes = webm.DecodeFrame(frame, webmInfo->ctx, webmInfo->ctxAlpha);
    xx::Data d;
    svpng(d, webm.width, webm.height, bytes.data(), 1);
    std::vector<u_int8_t> png;
    png.resize(d.len);
    memcpy(png.data(), d.buf, d.len);
    webmInfo->pngs.push_back(std::move(png));
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
        std::stringstream ss;
        ss << "load from webm fail: " << r;
        print_log(ss.str());
        return nullptr;
    }
    webm.ForeachFrame([webmInfo, &webm](std::vector<uint8_t> const& bytes)->int {
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
    return (int)info->webm.count;
}

int png_count(void* ptr) {
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

