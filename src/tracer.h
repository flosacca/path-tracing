#pragma once
#include "scene.h"

class RayTracer {
private:
    const Scene& scene;
    Sampler& a;
    const std::function<Vec(const Ray&)> f;
    int d = 0;

    struct Guard {
        RayTracer* tracer;

        ~Guard() {
            --tracer->d;
        }
    };

public:
    RayTracer(const Scene& scene, Sampler& a) :
        scene(scene), a(a), f([this] (const Ray& r) {
            return radiance(r);
        }) {}

    Vec radiance(const Ray& r) {
        Model::Detail s;
        scene.find(r, s);
        if (s.i.t == INF) {
            return Vec(0);
        }
        const Model::Meta& meta = s.m->m;
        Guard g {this};
        ++d;
        double p = 1;
        if (d >= 5) {
            p = glm::compMax(meta.c);
            if (d >= 20) {
                p = std::min(p, 0.9);
            }
            if (a.uniform() > p) {
                return meta.e;
            }
        }
        return meta.e + meta.c / p * meta.m.radiance({d, r, s.i, a, f});
    }
};
