#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>
#include "image.h"

Image::Image(int w, int h, uint8_t* p) : Image(w, h) {
    a.resize(w * h);
    for (int i = 0; i < w * h; ++i) {
        uint8_t* s = p + i * 3;
        a[i] = glm::pow(Vec(s[0], s[1], s[2]) / 255.0, Vec(2.2));
    }
    stbi_image_free(p);
}

Image Image::load(const std::string& path) {
    int w, h, c;
    uint8_t* p = stbi_load(path.data(), &w, &h, &c, 3);
    return Image(w, h, p);
}

void Image::save(const std::string& path) const {
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
