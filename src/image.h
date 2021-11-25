#pragma once
#include "ext/image.h"
#include "common.h"

class Image {
private:
    int w, h;
    std::vector<Vec> a;

    Image(int w, int h, uint8_t* p) : Image(w, h) {
        a.resize(w * h);
        for (int i = 0; i < w * h; ++i) {
            uint8_t* s = p + i * 3;
            a[i] = glm::pow(Vec(s[0], s[1], s[2]) / 255.0, Vec(2.2));
        }
        stbi_image_free(p);
    }

public:
    Image() {}

    Image(int w, int h) : w(w), h(h), a(w * h) {}

    static Image load(const std::string& path) {
        int w, h, c;
        uint8_t* p = stbi_load(path.data(), &w, &h, &c, 3);
        return Image(w, h, p);
    }

    int width() const {
        return w;
    }

    int height() const {
        return h;
    }

    void set(int i, int j, const Vec& c) {
        a[w * j + i] = c;
    }

    Vec sample(double u, double v) const {
        int i = u * (w - 1) + 0.5;
        int j = v * (h - 1) + 0.5;
        return a[w * (h - j - 1) + i];
    }

    void save(const std::string& path) const {
        uint8_t* p = (uint8_t*) malloc(w * h * 3);
        for (int i = 0; i < w * h; ++i) {
            uint8_t* s = p + i * 3;
            Vec c = glm::pow(a[i], Vec(1 / 2.2)) * 255.0 + 0.5;
            s[0] = c.r;
            s[1] = c.g;
            s[2] = c.b;
        }
        stbi_write_png(path.data(), w, h, 3, p, 0);
        free(p);
    }
};
