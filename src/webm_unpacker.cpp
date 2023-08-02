#include "webm_unpacker.h"

#include "xx_webm.h"

struct WebmInfo {
    struct xx::Webm webm;
    std::vector<std::vector<uint8_t>> frames;
    std::vector<bool> frameLoaded;
    vpx_codec_iface* iface{};
    vpx_codec_dec_cfg_t cfg{};
    vpx_codec_ctx_t ctx{};
    vpx_codec_ctx_t ctxAlpha{};
    uint8_t const* rgbBuf = nullptr;
    uint8_t const* aBuf = nullptr;
    uint32_t rgbBufLen = 0;
    uint32_t aBufLen = 0;
    bool loadFinish = false;
    int loadErrCode = 0;
    bool flipY = false;
};

static void* print_cb;

void set_debug_log_cb(log_cb cb) {
    print_cb = (void*)cb;
}

void print_log(const std::string& log) {
    if (print_cb == nullptr)
    {
        std::cout << "webm log:" << log << std::endl;
        return;
    }
    ((log_cb)print_cb)(log.c_str());
}

int init_decoder(void* ptr) {
    auto info = (WebmInfo*)ptr;
    auto& webm = info->webm;

    assert(webm.codecId);
    info->cfg = {1, webm.width, webm.height};
    auto iface = (vpx_codec_iface*)vpx_codec_vp9_dx();
    info->iface = iface;
    if (int r = vpx_codec_dec_init(&info->ctx, info->iface, &info->cfg, 0)) return 10000 + r;
//    if (int r = vpx_codec_decode(&info->ctx, info->rgbBuf, info->rgbBufLen, nullptr, 0)) assert(false);
    if (webm.hasAlpha) {
        if (int r = vpx_codec_dec_init(&info->ctxAlpha, info->iface, &info->cfg, 0)) return 20000 + r;
//        if (int r = vpx_codec_decode(&info->ctxAlpha, info->aBuf, info->aBufLen, nullptr, 0)) assert(false);
    }
    return 0;
}

void destroy_decoder(void* ptr) {
    auto info = (WebmInfo*)ptr;
    auto& webm = info->webm;

    vpx_codec_destroy(&info->ctx);
    if (webm.hasAlpha) {
        vpx_codec_destroy(&info->ctxAlpha);
    }
}

std::vector<uint8_t> decode_frame(struct xx::Webm& webm, vpx_codec_ctx_t& ctx,
        vpx_codec_ctx_t& ctxAlpha, int frame) {
    auto bytes = webm.DecodeFrame(frame, ctx, ctxAlpha);
    return bytes;
}

void flipY(WebmInfo* info, std::vector<uint8_t>& bytes) {
    auto data = bytes.data();
    auto width = info->webm.width;
    auto rowBytes = width * 4;
    auto rowCount = bytes.size() / rowBytes;

    uint8_t tmp[rowBytes];
    for (int i = 0; i < rowCount / 2; ++i) {
        auto firstRowIndex = i;
        auto lastRowIndex = rowCount - i - 1;

        auto firstRowDataIndex = data + firstRowIndex * rowBytes;
        auto lastRowDataIndex = data + lastRowIndex * rowBytes;

        memcpy(tmp, firstRowDataIndex, rowBytes);
        memcpy(firstRowDataIndex, lastRowDataIndex, rowBytes);
        memcpy(lastRowDataIndex, tmp, rowBytes);
    }
}

void save_frame(WebmInfo* info, std::vector<uint8_t>&& bytes, int index) {
    if (info->flipY) flipY(info, bytes);
    info->frames[index] = std::move(bytes);
    info->frameLoaded[index] = true;
}

int load_frame(WebmInfo* webmInfoCtx, uint8_t* data, int len, int threadIndex, int threadCount) {
//    auto info = std::make_unique<WebmInfo>();
    WebmInfo webmInfo;
    auto info = &webmInfo;
    auto& webm = info->webm;
    auto result = std::make_unique<std::uint8_t[]>(len);
    memcpy(result.get(), data, len);

    if (int r = webm.LoadFromWebm(std::move(result), len)) {
        std::stringstream ss;
        ss << "load from webm fail: " << r;
        print_log(ss.str());

        info->loadErrCode = r;
        info->loadFinish = true;
        return r + 1000;
    }
    if (int r = init_decoder(info)) {
        info->loadErrCode = r;
        info->loadFinish = true;
        return r + 2000;
    }

    std::cout << "start:" << threadIndex << std::endl;
    auto count = (int)info->webm.count;
    for (auto i = 0; i < count; i++) {
        if (i % threadCount != threadIndex) {
            webm.SkipFrame(i, info->ctx, info->ctxAlpha);
            continue;
        }
        auto bytes = decode_frame(webm, info->ctx, info->ctxAlpha, i);
        save_frame(webmInfoCtx, std::move(bytes), i);
//        webmInfoCtx->frames[i] = std::move(decode_frame(webm, info->ctx, info->ctxAlpha, i));
//        webmInfoCtx->frameLoaded[i] = true;
    }
    return 0;
}

int init_load_webm(WebmInfo* info, uint8_t* data, int len) {
    auto result = std::make_unique<std::uint8_t[]>(len);
    memcpy(result.get(), data, len);
    if (int r = info->webm.LoadFromWebm(std::move(result), len)) {
        std::stringstream ss;
        ss << "load from webm fail: " << r;
        print_log(ss.str());
        return r + 1000;
    }
    if (int r = init_decoder(info)) {
        return r + 2000;
    }
    return 0;
}

void load_webm(WebmInfo* info, uint8_t* data, int len, bool loadFrames, int loadFramesThreadCount) {
    if (loadFrames) {
        auto time1 = std::chrono::steady_clock::now();
        if (loadFramesThreadCount > 1) {
            auto& webm = info->webm;
            if (int r = init_load_webm(info, data, len)) {
                info->loadErrCode = r;
            }

            auto count = webm.count;
            info->frames.resize(count);
            info->frameLoaded.resize(count, false);
            info->loadFinish = true;

            std::vector<std::thread> threads(loadFramesThreadCount);
            for (auto i = 0; i < loadFramesThreadCount; ++i) {
                threads[i] = std::thread(load_frame, info, data, len, i, loadFramesThreadCount);
                threads[i].detach();
            }
        } else {
            if (int r = init_load_webm(info, data, len)) {
                info->loadErrCode = r;
            } else {
                info->loadErrCode = 0;
            }
            info->loadFinish = true;
            
            auto& webm = info->webm;
            auto count = webm.count;
            info->frames.resize(count);
            info->frameLoaded.resize(count, false);

            xx::Data d;
            webm.ForeachFrame([info, &webm, &d](std::vector<uint8_t>& bytes, int index)->int {
                d.Clear();
//                info->frames[index] = std::move(bytes);
//                info->frameLoaded[index] = true;
                save_frame(info, std::move(bytes), index);
                return 0;
            });
        }
        auto time2 = std::chrono::steady_clock::now();
        auto duration11 = std::chrono::duration_cast<std::chrono::microseconds>(time2- time1);
        std::cout << "load webm frame: use " << duration11.count()
        << " microseconds, threads count: " << loadFramesThreadCount << std::endl;
    }
}

void* create_webm_decoder(uint8_t* data, int len, bool loadFrames, int loadFramesThreadCount, bool flipY) {
    auto webmInfo = new WebmInfo();
    webmInfo->flipY = flipY;
    auto load = std::thread(load_webm, webmInfo, data, len, loadFrames, loadFramesThreadCount);
    load.detach();
    return (void*)webmInfo;
}

bool is_load_finish(void* ptr) {
    auto webmInfo = (WebmInfo*)ptr;
    return webmInfo->loadFinish;
}

bool is_frame_load_finish(void* ptr, int frame) {
    auto webmInfo = (WebmInfo*)ptr;
    return webmInfo->frameLoaded[frame];
}

int load_err_code(void* ptr) {
    auto webmInfo = (WebmInfo*)ptr;
    return webmInfo->loadErrCode;
}

void decode_frame(void* ptr, int frame) {
    auto webmInfo = (WebmInfo*)ptr;
    auto& webm = webmInfo->webm;
    auto bytes = webm.DecodeFrame(frame, webmInfo->ctx, webmInfo->ctxAlpha);
//    xx::Data d;
//    svpng(d, webm.width, webm.height, bytes.data(), 1);
//    std::vector<uint8_t> png;
//    png.resize(d.len);
//    memcpy(png.data(), d.buf, d.len);
//    webmInfo->frames.push_back(std::move(png));
//    webmInfo->frames[frame] = std::move(png);
//    webmInfo->frames[frame] = std::move(bytes);
//    webmInfo->frameLoaded[frame] = true;
    save_frame(webmInfo, std::move(bytes), frame);
}

void* decode_webm(const char *webmPath) {
    auto webmInfo = new WebmInfo();
    auto& webm = webmInfo->webm;
    int r = webm.LoadFromWebmWithPath(webmPath);
    if (r) {
        return nullptr;
    }
    webm.ForeachFrame([&](std::vector<uint8_t> const& bytes, int index)->int {
        xx::Data d;
        svpng(d, webm.width, webm.height, bytes.data(), 1);
        std::vector<uint8_t> png;
        png.resize(d.len);
        memcpy(png.data(), d.buf, d.len);
        webmInfo->frames.push_back(std::move(png));
        return 0;
    });
    return (void*)webmInfo;
}

void* decode_webm_by_data(uint8_t* data, int len) {
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
    webm.ForeachFrame([webmInfo, &webm](std::vector<uint8_t> const& bytes, int index)->int {
        xx::Data d;
        svpng(d, webm.width, webm.height, bytes.data(), 1);
        std::vector<uint8_t> png;
        png.resize(d.len);
        memcpy(png.data(), d.buf, d.len);
        webmInfo->frames.push_back(std::move(png));
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

int abi_version(void* ptr) {
    auto info = (WebmInfo*)ptr;
//    return info->iface.abi_version;
    return 0;
}

int png_count(void* ptr) {
    auto info = (WebmInfo*)ptr;
    return (int)info->frames.size();
}

int get_webm_width(void* ptr) {
    auto info = (WebmInfo*)ptr;
    return info->webm.width;
}

int get_webm_height(void* ptr){
    auto info = (WebmInfo*)ptr;
    return info->webm.height;
}

uint8_t* get_frame_data(void* ptr, int frame) {
    auto info = (WebmInfo*)ptr;
    return info->frames[frame].data();
}

std::vector<uint8_t> get_raw_frame_data(void* ptr, int frame) {
    auto info = (WebmInfo*)ptr;
    return info->frames[frame];
}

int get_frame_data_size(void* ptr, int frame)
{
    auto info = (WebmInfo*)ptr;
    return (int)info->frames[frame].size();
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

int unpack_webm1(uint8_t *data, int size, const char *outPath, const char *prefix) {
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

