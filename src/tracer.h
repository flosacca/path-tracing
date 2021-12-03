#pragma once
#include "scene.h"
#include "sampler.h"
#include "option.h"

class RayTracer {
private:
    const Scene& scene;
    Sampler& a;
    Option opt;
    int d = 0;
    float q = 1;

    struct Visitor {
        RayTracer& self;
        Ray r;
        Vec n;

        Vec visit(const Material::Diffuse& m) {
            float l = glm::dot(r.d, n);
            float phi = self.a.uniform(2 * PI);
            float rho = glm::sqrt(self.a.uniform());
            Vec d = UnitVec::from_rho(phi, rho, std::copysign(1.0f, -l) * n);
            return self.radiance(Ray(r.o, d));
        }

        Vec visit(const Material::Specular& m) {
            float l = glm::dot(r.d, n);
            Vec d = r.d - 2.0f * l * n;
            return self.radiance(Ray(r.o, d));
        }

        Vec visit(const Material::Refractive& m) {
            float l = glm::dot(r.d, n);
            constexpr float n0 = 1;
            float eta = std::signbit(l) ? n0 / m.n : m.n / n0;
            float k2 = 1 - eta * eta * (1 - l * l);
            Vec d = r.d - 2.0f * l * n;
            if (cmp::lessEqual(k2, 0)) {
                return self.radiance(Ray(r.o, d));
            }
            float k = glm::sqrt(k2);
            Vec t = eta * r.d - (eta * l + std::copysign(k, -l)) * n;
            float R0 = num::pow<2>((m.n - n0) / (m.n + n0));
            float R = R0 + (1 - R0) * num::pow<5>(1 - std::min(k, glm::abs(l)));
            if (self.d <= 2) {
                return R * self.radiance(Ray(r.o, d)) + (1 - R) * self.radiance(Ray(r(cmp::EPS * 2), t));
            }
            float p = 0.25f + 0.5f * R;
            if (self.a.uniform() < p) {
                return self.radiance(Ray(r.o, d)) * (R / p);
            } else {
                return self.radiance(Ray(r(cmp::EPS * 2), t)) * ((1 - R) / (1 - p));
            }
        }
    };

public:
    RayTracer(const Scene& s, Sampler& a, const Option& opt) :
        scene(s), a(a), opt(opt) {}

    Vec radiance(const Ray& r) {
        Model::Detail s;
        scene.find(r, s);
        if (s.t == INF) {
            return Vec(0);
        }
        Vec e = q * s.e;
        if (d >= opt.bounces) {
            if (!opt.rr) {
                return e;
            }
            float p = d >= opt.rr_bounces ? opt.rr : glm::compMax(s.c);
            if (a.uniform() > p) {
                return e;
            }
            q /= p;
        }
        Visitor v {*this, Ray(r(s.t - cmp::EPS), r.d), s.n};
        float q0 = q;
        ++d;
        e += s.c * s.m.radiance(v);
        --d;
        q = q0;
        return e;
    }
};
