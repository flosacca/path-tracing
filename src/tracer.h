#pragma once
#include "scene.h"
#include "sampler.h"

class RayTracer {
private:
    const Scene& scene;
    Sampler& a;
    int d = 0;

    struct Guard {
        RayTracer* tracer;

        ~Guard() {
            --tracer->d;
        }
    };

    struct Visitor final : Material::Visitor {
        RayTracer& self;
        Ray r;
        Vec n;

        Visitor(RayTracer& self, const Ray& r, const Vec& n) :
            self(self), r(r), n(n) {}

        Vec visit(const Material::Diffuse& m) final {
            double l = glm::dot(r.d, n);
            double phi = self.a.uniform(2 * PI);
            double rho = glm::sqrt(self.a.uniform());
            Vec d = num::unit::from_rho(phi, rho, std::copysign(1.0, -l) * n);
            return self.radiance(Ray(r.o, d));
        }

        Vec visit(const Material::Specular& m) final {
            double l = glm::dot(r.d, n);
            Vec d = r.d - 2.0 * l * n;
            return self.radiance(Ray(r.o, d));
        }

        Vec visit(const Material::Refraction& m) final {
            double l = glm::dot(r.d, n);
            constexpr double n0 = 1;
            double eta = std::signbit(l) ? n0 / m.n : m.n / n0;
            double k2 = 1 - eta * eta * (1 - l * l);
            Vec d = r.d - 2.0 * l * n;
            if (num::lessEqual(k2, 0)) {
                return self.radiance(Ray(r.o, d));
            }
            double k = glm::sqrt(k2);
            Vec t = eta * r.d - (eta * l + std::copysign(k, -l)) * n;
            double R0 = num::pow<2>((m.n - n0) / (m.n + n0));
            double R = R0 + (1 - R0) * num::pow<5>(1 - std::min(k, glm::abs(l)));
            if (self.d <= 2) {
                return R * self.radiance(Ray(r.o, d)) + (1 - R) * self.radiance(Ray(r.o, t));
            }
            double p = 0.25 + 0.5 * R;
            if (self.a.uniform() < p) {
                return self.radiance(Ray(r.o, d)) * (R / p);
            } else {
                return self.radiance(Ray(r.o, t)) * ((1 - R) / (1 - p));
            }
        }
    };

public:
    RayTracer(const Scene& scene, Sampler& a) :
        scene(scene), a(a) {}

    Vec radiance(const Ray& r) {
        Model::Detail s;
        scene.find(r, s);
        if (s.t == INF) {
            return Vec(0);
        }
        Guard g {this};
        ++d;
        double p = 1;
        if (d >= 5) {
            p = glm::compMax(s.c);
            if (d >= 20) {
                p = std::min(p, 0.9);
            }
            if (a.uniform() > p) {
                return s.e;
            }
        }
        Visitor v(*this, Ray(r(s.t), r.d), s.n);
        return s.e + s.c / p * s.m.radiance(v);
    }
};
