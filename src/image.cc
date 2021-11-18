#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>
#include "image.h"

Image::Image(int w, int h) :
    w(w), h(h), a((uint8_t*) malloc(w * h * 3), [] (uint8_t* a) {
        free(a);
    }) {}

Image Image::load(const std::string& path) {
    int w, h, c;
    uint8_t* a = stbi_load(path.data(), &w, &h, &c, 3);
    return Image(w, h, {a, [] (uint8_t* a) {
        stbi_image_free(a);
    }});
}

void Image::save(const std::string& path) const {
    stbi_write_png(path.data(), w, h, 3, a.get(), 0);
}
