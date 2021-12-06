#pragma once
#include "ext/image.h"
#include "common.h"

class Image {
private:
    size_t w = 0;
    size_t h = 0;
    std::vector<Vec> a;

    Image(size_t w, size_t h, uint8_t* p) : Image(w, h) {
        a.resize(w * h);
        for (size_t i = 0; i < w * h; ++i) {
            uint8_t* s = p + i * 3;
            a[i] = glm::pow(Vec(s[0], s[1], s[2]) / 255.0, Vec(2.2));
        }
        stbi_image_free(p);
    }

public:
    Image() = default;

    Image(size_t w, size_t h) : w(w), h(h), a(w * h) {}

    static Image load(const std::string& path) {
        int w, h, c;
        uint8_t* p = stbi_load(path.data(), &w, &h, &c, 3);
        return Image(w, h, p);
    }

    size_t width() const {
        return w;
    }

    size_t height() const {
        return h;
    }

    explicit operator bool() const {
        return a.size();
    }

    void set(size_t i, size_t j, const Vec& c) {
        a[w * j + i] = c;
    }

    Vec sample(double u, double v) const {
        size_t i = u * (w - 1) + 0.5;
        size_t j = v * (h - 1) + 0.5;
        return a[w * (h - j - 1) + i];
    }

    void save_png(const std::string& path) const {
        uint8_t (*p)[3] = (uint8_t(*)[3]) malloc(w * h * 3);
        for (size_t i = 0; i < w * h; ++i) {
            Vec c = glm::pow(a[i], Vec(1 / 2.2)) * 255.0 + 0.5;
            p[i][0] = c.r;
            p[i][1] = c.g;
            p[i][2] = c.b;
        }
        stbi_write_png(path.data(), w, h, 3, p, 0);
        free(p);
    }

    void save_raw(const std::string& path) const {
        size_t D = sizeof(float[3]);
        float (*p)[3] = (float(*)[3]) malloc(w * h * D);
        for (size_t i = 0; i < w * h; ++i) {
            Vec c = glm::pow(a[i], Vec(1 / 2.2));
            p[i][0] = c.r;
            p[i][1] = c.g;
            p[i][2] = c.b;
        }
        FILE* f = fopen(path.data(), "wb");
        fwrite(p, D, w * h, f);
        fclose(f);
        free(p);
    }
};
