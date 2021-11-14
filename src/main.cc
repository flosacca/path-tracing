#include "mesh.h"
#include "camera.h"
#include "device.h"

int main(int argc, char** argv) {
    int w = 200;
    int h = 150;
    int spp = argc >= 2 ? atoi(argv[1]) : 4;
    if (argc >= 4) {
        w = atoi(argv[2]);
        h = atoi(argv[3]);
    }
    std::vector<Vec> vertices {
        {+1, +1, +1},
        {-1, +1, +1},
        {-1, -1, +1},
        {+1, -1, +1},
        {+1, +1, -1},
        {-1, +1, -1},
        {-1, -1, -1},
        {+1, -1, -1},
    };
    std::vector<Mesh::Index> indices {
        {1, 3, 0},
        {1, 2, 3},
        {0, 5, 1},
        {0, 4, 5},
        {1, 6, 2},
        {1, 5, 6},
        {2, 7, 3},
        {2, 6, 7},
        {3, 4, 0},
        {3, 7, 4},
        {6, 4, 7},
        {6, 5, 4},
    };
    glm::dmat4 t(1.0);
    t = glm::translate(t, Vec(0, 50, 50));
    t = glm::rotate(t, PI / 4, Vec(1, 1, 1));
    t = glm::scale(t, Vec(8));
    for (Vec& p : vertices) {
        p = Vec(t * glm::dvec4(p, 1.0));
        printf("(%6.2f, %6.2f, %6.2f)\n", p.x, p.y, p.z);
    }
    Scene scene {
        new Plane(Vec(-1, 0, 0), Vec(-50, 0, 0), Vec(0.75, 0.25, 0.25)),
        new Plane(Vec(+1, 0, 0), Vec(+50, 0, 0), Vec(0.25, 0.25, 0.75)),
        new Plane(Vec(0, 0, +1), Vec(0, 0, 0), Vec(0.75), Material::DIFFUSE),
        new Plane(Vec(0, 0, -1), Vec(0, 0, 225), Vec(0.25, 0.75, 0.25)),
        new Plane(Vec(0, +1, 0), Vec(0, 0, 0), Vec(0.75)),
        new Plane(Vec(0, -1, 0), Vec(0, 100, 0), Vec(0.75)),
        new Sphere(16, Vec(-25, 16, 25), Vec(1), Material::SPECULAR),
        new Sphere(16, Vec(+25, 16, 50), Vec(1), Material::REFRACTION),
        // new Sphere(600, Vec(0, 700 - 0.3, 50), Vec(0), Material::DIFFUSE, Vec(12)),
        new Sphere(1.5, Vec(0, 80, 50), Vec(0), Material::DIFFUSE, Vec(400)),
        new Mesh(vertices, indices, Vec(0.25, 0.75, 0.25)),
    };
    Camera cam(Vec(0, 50, 225), Vec(0, 0, -1), (double) w / h);
    Image im(w, h);
    cam.render(scene, im, spp);
    im.save("output.png");
}
