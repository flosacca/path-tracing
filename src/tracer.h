#pragma once
#include "scene.h"

class Sampler {
private:
    std::mt19937 g;

public:
    Sampler(uint32_t s) : g(s) {}

    double uniform() {
        return std::uniform_real_distribution<double>()(g);
    }

    double triangle() {
        double x = 2 * uniform() - 1;
        return (1 - glm::sqrt(1 - glm::abs(x))) * glm::sign(x);
    }
};

class RayTracer {
private:
    const Scene& scene;
    Sampler& samp;

public:
    RayTracer(const Scene& scene, Sampler& samp) :
        scene(scene), samp(samp) {}

    Vec radiance(const Ray& r, int depth) {
        Intersection i = scene.find(r);
        if (!i.m) {
            return Vec(0);
        }
        double t = i.t;
        Vec n = i.n;
        Vec x = r(t);
        bool into = glm::dot(n, r.d) < 0;
        Vec nl = into ? n : -n;
        Vec f = i.m->c;
        double p = glm::compMax(f);
        if (depth >= 20) {
            p = std::min(p, 0.9);
        }
        ++depth;
        if (depth > 5) {
            if (samp.uniform() > p) {
                return i.m->e;
            }
            f /= p;
        }
        if (i.m->m == Material::DIFFUSE) {
            double r1 = 2 * PI * samp.uniform();
            double r2 = samp.uniform();
            double r2s = sqrt(r2);
            Vec w = nl;
            double theta = glm::acos(w.z / glm::length(w));
            double phi = glm::atan(w.y, w.x);
            theta = fmod(theta + PI / 2, PI);
            Vec u = Vec {
                glm::cos(phi) * glm::sin(theta),
                glm::sin(phi) * glm::sin(theta),
                glm::cos(theta)
            };
            Vec v = glm::cross(w, u);
            Vec d = glm::normalize(u * glm::cos(r1) * r2s + v * glm::sin(r1) * r2s + w * glm::sqrt(1 - r2));
            return i.m->e + f * radiance(Ray(x, d), depth);
        }
        Ray reflRay(x, glm::reflect(r.d, n));
        if (i.m->m == Material::SPECULAR) {
            return i.m->e + f * radiance(reflRay, depth);
        }
        double nc = 1, nt = 1.5;
        double eta = nc / nt;
        double nnt = into ? eta : 1 / eta;
        double ddn = glm::dot(r.d, nl);
        double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
        if (cos2t < 0) {
            return i.m->e + f * radiance(reflRay, depth);
        }
        Vec tdir = glm::normalize(r.d * nnt - nl * (ddn * nnt + glm::sqrt(cos2t)));
        double a = nt - nc;
        double b = nt + nc;
        double R0 = a * a / (b * b);
        double c = 1 - (into ? -ddn : glm::dot(tdir, n));
        double Re = R0 + (1 - R0) * glm::pow(c, 5);
        double Tr = 1 - Re;
        double P = 0.25 + 0.5 * Re;
        if (depth <= 2) {
            return i.m->e + f * (radiance(reflRay, depth) * Re + radiance(Ray(x, tdir), depth) * Tr);
        }
        return i.m->e + f * (samp.uniform() < P ? radiance(reflRay, depth) * (Re / P) : radiance(Ray(x, tdir), depth) * (Tr / (1 - P)));
    }
};
