#pragma once
#include "common.h"

class Image {
private:
    const int w;
    const int h;
    std::shared_ptr<uint8_t> p;

    Image(int w, int h, std::shared_ptr<uint8_t> p) :
        w(w), h(h), p(p) {}

public:
    Image(int w, int h);

    int width() const {
        return w;
    }

    int height() const {
        return h;
    }

    void set(int x, int y, const Vec& hdr) {
        Vec rgb = glm::pow(hdr, Vec(1 / 2.2)) * 255.0 + 0.5;
        uint8_t* s = p.get() + ((h - y - 1) * w + x) * 3;
        s[0] = rgb.r;
        s[1] = rgb.g;
        s[2] = rgb.b;
    }

    void save(const std::string& path) const;

    static Image load(const std::string& path);
};
