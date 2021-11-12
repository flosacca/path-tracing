#include "camera.h"
#include "device.h"

int main(int argc, char** argv) {
    int w = 160;
    int h = 120;
    int spp = argc >= 2 ? atoi(argv[1]) : 100;
    if (argc >= 4) {
        w = atoi(argv[2]);
        h = atoi(argv[3]);
    }
    Scene scene {
        new Plane(Vec(-1, 0, 0), Vec(-50, 0, 0), Vec(0.75, 0.25, 0.25)),
        new Plane(Vec(+1, 0, 0), Vec(+50, 0, 0), Vec(0.25, 0.25, 0.75)),
        new Plane(Vec(0, 0, +1), Vec(0, 0, 0), Vec(0.75)),
        new Plane(Vec(0, 0, -1), Vec(0, 0, 225), Vec(0.25, 0.75, 0.25)),
        new Plane(Vec(0, +1, 0), Vec(0, 0, 0), Vec(0.75)),
        new Plane(Vec(0, -1, 0), Vec(0, 100, 0), Vec(0.75)),
        new Sphere(16, Vec(-25, 16, 25), Vec(1), Material::SPECULAR),
        new Sphere(16, Vec(+25, 16, 50), Vec(1), Material::REFRACTION),
        new Sphere(600, Vec(0, 700 - 0.3, 50), Vec(0), Material::DIFFUSE, Vec(12)),
    };
    Camera cam(Vec(0, 50, 225), Vec(0, 0, -1), (double) w / h);
    Image im(w, h);
    cam.render(scene, im, spp);
    im.save("output.png");
}
