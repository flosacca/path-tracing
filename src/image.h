#pragma once
#include "common.h"

class Image {
private:
    int w, h;
    std::vector<Vec> a;

    Image(int w, int h, uint8_t* p);

public:
    Image() {}

    Image(int w, int h) :
        w(w), h(h), a(w * h) {}

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

    void save(const std::string& path) const;

    static Image load(const std::string& path);
};
