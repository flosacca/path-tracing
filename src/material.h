#pragma once
#include "sampler.h"

struct Material {
    struct Tracing {
        int depth;
        const Ray& ray;
        const Intersection& i;
        Sampler& sampler;
        const std::function<Vec(const Ray&)>& callback;

        Vec operator()(const Ray& r) const {
            return callback(r);
        }
    };

    struct Base {
        virtual Vec radiance(Tracing&&) const = 0;
    };

    struct Diffuse : Base {
        Vec radiance(Tracing&& tracing) const override {
            auto& r = tracing.ray;
            auto& a = tracing.sampler;
            Vec n = tracing.i.n;
            Vec x = r(tracing.i.t);
            double l = glm::dot(r.d, n);
            double phi = a.uniform(2 * PI);
            double rho = glm::sqrt(a.uniform());
            Vec d = num::unit::from_rho(phi, rho, std::copysign(1.0, -l) * n);
            return tracing(Ray(x, d));
        }
    };

    struct Specular : Base {
        Vec radiance(Tracing&& tracing) const override {
            auto& r = tracing.ray;
            Vec n = tracing.i.n;
            Vec x = r(tracing.i.t);
            double l = glm::dot(r.d, n);
            Vec d = r.d - 2.0 * l * n;
            return tracing(Ray(x, d));
        }
    };

    struct Refraction : Base {
        double n;

        Refraction(double n = 1.5) : n(n) {}

        Vec radiance(Tracing&& tracing) const override {
            auto& r = tracing.ray;
            Vec n = tracing.i.n;
            Vec x = r(tracing.i.t);
            double l = glm::dot(r.d, n);
            constexpr double n0 = 1;
            double eta = std::signbit(l) ? n0 / this->n : this->n / n0;
            double k2 = 1 - eta * eta * (1 - l * l);
            Vec d = r.d - 2.0 * l * n;
            if (num::lessEqual(k2, 0)) {
                return tracing(Ray(x, d));
            }
            double k = glm::sqrt(k2);
            Vec t = eta * r.d - (eta * l + std::copysign(k, -l)) * n;
            double R0 = num::pow<2>((this->n - n0) / (this->n + n0));
            double R = R0 + (1 - R0) * num::pow<5>(1 - std::min(k, glm::abs(l)));
            if (tracing.depth <= 2) {
                return R * tracing(Ray(x, d)) + (1 - R) * tracing(Ray(x, t));
            }
            double p = 0.25 + 0.5 * R;
            auto& a = tracing.sampler;
            if (a.uniform() < p) {
                return tracing(Ray(x, d)) * (R / p);
            } else {
                return tracing(Ray(x, t)) * ((1 - R) / (1 - p));
            }
        }
    };

    std::shared_ptr<const Base> p;

    Vec radiance(Tracing&& tracing) const {
        return p->radiance(std::forward<Tracing>(tracing));
    }

    template <typename... Args>
    static Material diffuse(Args&&... args) {
        return {std::make_shared<Diffuse>(std::forward<Args>(args)...)};
    }

    template <typename... Args>
    static Material specular(Args&&... args) {
        return {std::make_shared<Specular>(std::forward<Args>(args)...)};
    }

    template <typename... Args>
    static Material refraction(Args&&... args) {
        return {std::make_shared<Refraction>(std::forward<Args>(args)...)};
    }
};
