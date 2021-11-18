#pragma once
#include "common.h"

class Image {
private:
    int w, h;
    std::shared_ptr<uint8_t> a;

    Image(int w, int h, std::shared_ptr<uint8_t> a) :
        w(w), h(h), a(a) {}

public:
    Image() {}

    Image(int w, int h);

    int width() const {
        return w;
    }

    int height() const {
        return h;
    }

    uint8_t* pixel(int i, int j) {
        return a.get() + (w * j + i) * 3;
    }

    const uint8_t* pixel(int i, int j) const {
        return const_cast<Image*>(this)->pixel(i, j);
    }

    void set(int i, int j, const Vec& c) {
        Vec s = glm::pow(c, Vec(1 / 2.2)) * 255.0 + 0.5;
        uint8_t* p = pixel(i, j);
        p[0] = s.r;
        p[1] = s.g;
        p[2] = s.b;
    }

    Vec get(double x, double y) const {
        int i = x * (w - 1) + 0.5;
        int j = y * (h - 1) + 0.5;
        const uint8_t* p = pixel(i, h - j - 1);
        return glm::pow(Vec(p[0], p[1], p[2]) / 255.0, Vec(2.2));
    }

    void save(const std::string& path) const;

    static Image load(const std::string& path);
};
