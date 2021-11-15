#pragma once
#include "scene.h"

class RayTracer {
private:
    const Scene& scene;
    Sampler& a;
    const std::function<Vec(const Ray&)> f;
    int d = 0;

    struct Guard {
        RayTracer* p;
        Guard(RayTracer* p) : p(p) { ++p->d; }
        ~Guard() { --p->d; }
    };

public:
    RayTracer(const Scene& scene, Sampler& a) :
        scene(scene), a(a), f([this] (const Ray& r) {
            return radiance(r);
        }) {}

    Vec radiance(const Ray& r) {
        Guard g(this);
        const Intersection s = scene.find(r);
        if (!s.m) {
            return Vec(0);
        }
        Vec c = s.m->c;
        double p = glm::compMax(c);
        if (d >= 20) {
            p = std::min(p, 0.9);
        }
        if (d >= 5) {
            if (a.uniform() > p) {
                return s.m->e;
            }
            c /= p;
        }
        return s.m->e + c * s.m->m.radiance({d, r, s, a, f});
    }
};
