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

    static Vec ortho(const Vec& w) {
        double theta = glm::acos(w.z / glm::length(w));
        double phi = glm::atan(w.y, w.x);
        theta = fmod(theta + PI / 2, PI);
        return {
            glm::cos(phi) * glm::sin(theta),
            glm::sin(phi) * glm::sin(theta),
            glm::cos(theta)
        };
    }

    Vec radiance(const Ray& r, int depth, double E = 1) {
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
            Vec u = ortho(w);
            Vec v = glm::cross(w, u);
            Vec d = glm::normalize(u * glm::cos(r1) * r2s + v * glm::sin(r1) * r2s + w * glm::sqrt(1 - r2));
            Vec e(0);
            for (const auto& p : scene.models){
                Sphere* s = dynamic_cast<Sphere*>(p.get());
                if (!s) {
                    continue;
                }
                if (num::zero(glm::length(s->e))) {
                    continue;
                }
                Vec sd = s->o - x;
                Vec sw = glm::normalize(sd);
                Vec su = ortho(sw);
                Vec sv = glm::cross(sw, su);
                double cos_a_max = sqrt(1 - s->r * s->r / glm::dot(sd, sd));
                double ksi1 = samp.uniform();
                double ksi2 = samp.uniform();
                double cos_a = 1 + ksi1 * (cos_a_max - 1);
                double sin_a = sqrt(1 - cos_a * cos_a);
                double phi = 2 * PI * ksi2;
                Vec l = glm::normalize(su * glm::cos(phi) * sin_a + sv * glm::sin(phi) * sin_a + sd * cos_a);
                Intersection i2 = scene.find(Ray(x, l));
                if (i2.m == s) {
                    double omega = 2 * PI * (1 - cos_a_max);
                    e += f * s->e * (glm::dot(l, nl) * omega / PI);
                }
            }
            return i.m->e * E + e + f * radiance(Ray(x, d), depth, 0);
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
