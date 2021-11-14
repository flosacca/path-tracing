#include "mesh.h"
#include "camera.h"
#include "device.h"
#include "ply.h"

template <typename... Args>
Mesh* createMesh(const std::string& name, glm::dmat4 transform, Args&&... args) {
    std::vector<Vec> vertices;
    std::vector<Mesh::Index> indices;
    loadPly("models/stanford/" + name + ".ply", vertices, indices);
    for (Vec& p : vertices) {
        p = Vec(transform * glm::dvec4(p, 1.0));
        // printf("(%6.2f, %6.2f, %6.2f)\n", p.x, p.y, p.z);
    }
    return new Mesh(vertices, indices, std::forward<Args>(args)...);
};

template <typename F>
glm::dmat4 compose(F f) {
    glm::dmat4 t(1);
    f(t);
    return t;
}

int main(int argc, char** argv) {
    int w = 200;
    int h = w / 4 * 3;
#ifdef LIGHT_SAMPLING
    int spp = 4;
#else
    int spp = 100;
#endif
    if (argc >= 2) {
        spp = atoi(argv[1]);
    }
    if (argc >= 4) {
        w = atoi(argv[2]);
        h = atoi(argv[3]);
    }
    auto t = compose([] (auto& t) {
        t = glm::translate(t, Vec(0, 0, 50));
        // t = glm::rotate(t, PI / 4, Vec(1, 1, 1));
        t = glm::scale(t, Vec(150));
    });
    Scene scene {
        new Plane(Vec(-1, 0, 0), Vec(-50, 0, 0), Vec(0.75, 0.25, 0.25)),
        new Plane(Vec(+1, 0, 0), Vec(+50, 0, 0), Vec(0.25, 0.25, 0.75)),
        new Plane(Vec(0, 0, +1), Vec(0, 0, 0), Vec(0.75), Material::DIFFUSE),
        new Plane(Vec(0, 0, -1), Vec(0, 0, 225), Vec(0.25, 0.75, 0.25)),
        new Plane(Vec(0, +1, 0), Vec(0, 0, 0), Vec(0.75)),
        new Plane(Vec(0, -1, 0), Vec(0, 100, 0), Vec(0.75)),
        new Sphere(16, Vec(-25, 16, 25), Vec(1), Material::SPECULAR),
        new Sphere(16, Vec(+25, 16, 50), Vec(1), Material::REFRACTION),
#ifdef LIGHT_SAMPLING
        new Sphere(1.5, Vec(0, 80, 50), Vec(0), Material::DIFFUSE, Vec(400)),
#else
        new Sphere(600, Vec(0, 700 - 0.3, 50), Vec(0), Material::DIFFUSE, Vec(12)),
#endif
        createMesh("bun_zipper", t, Vec(0.831, 0.686, 0.216), Material::SPECULAR),
    };
    Camera cam(Vec(0, 50, 225), Vec(0, 0, -1), (double) w / h);
    Image im(w, h);
    cam.render(scene, im, spp);
    im.save("output.png");
}
