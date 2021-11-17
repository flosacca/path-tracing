#pragma once
#include "common.h"
#include "stb_image_write.h"

class Image {
private:
    const int w;
    const int h;
    std::vector<uint8_t> s;

public:
    Image(int w, int h) :
        w(w), h(h), s(w * h * 3) {}

    int width() const {
        return w;
    }

    int height() const {
        return h;
    }

    void set(int x, int y, const Vec& hdr) {
        Vec ldr = glm::pow(hdr, Vec(1 / 2.2)) * 255.0 + 0.5;
        uint8_t* p = s.data() + ((h - y - 1) * w + x) * 3;
        p[0] = ldr.x;
        p[1] = ldr.y;
        p[2] = ldr.z;
    }

    void save(const std::string& path) const {
        stbi_write_png(path.data(), w, h, 3, s.data(), 0);
    }
};
