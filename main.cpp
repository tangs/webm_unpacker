#include <iostream>

#include "xx_webm.h"

int main() {
//    auto path = "./res/1.webm";
    auto path = "/Users/tangs/Desktop/tmp/webm/mokey_vp9_60fps.webm";
    auto outputPath = "/Users/tangs/Desktop/tmp/webm/output";
    struct xx::Webm webm;
    if (int r = webm.LoadFromWebm(path)) {
        return r;
    }
    if (int r = webm.SaveToPngs(outputPath, "abc_")) {
        return r;
    }
    int a = 3;
    return 0;
}
