#pragma once
#include "material.h"

struct Model {
    struct Meta {
        Vec c;
        Vec e;
        Material m;
    };

    Meta m;

    template <typename... Args>
    Model(Args&&... args) :
        m {std::forward<Args>(args)...} {}

    virtual void find(const Ray&, Intersection&) const = 0;
};

struct Sphere : Model {
    double r;
    Vec o;

    Sphere(double r, const Vec& o, const Vec& c,
        Material m = Material::diffuse(), const Vec& e = Vec(0)) :
        Model(c, e, m), r(r), o(o) {}

    void find(const Ray& ray, Intersection& s) const override {
        Vec v = o - ray.o;
        double b = glm::dot(v, ray.d);
        double d2 = r * r - (glm::dot(v, v) - b * b);
        if (num::greater(d2, 0)) {
            double d = glm::sqrt(d2);
            double t1 = b + d;
            if (num::greater(t1, 0)) {
                double t2 = b - d;
                double t = num::greater(t2, 0) ? t2 : t1;
                if (t < s.t) {
                    Vec n = glm::normalize(ray(t) - o);
                    s = {t, n, this};
                }
            }
        }
    }
};

struct Plane : Model {
    Vec n, p;

    Plane(const Vec& n, const Vec& p, const Vec& c,
        Material m = Material::diffuse(), const Vec& e = Vec(0)) :
        Model(c, e, m), n(n), p(p) {}

    void find(const Ray& r, Intersection& s) const override {
        double d = glm::dot(r.d, n);
        if (num::nonzero(d)) {
            double t = glm::dot(p - r.o, n) / d;
            if (num::greater(t, 0) && t < s.t) {
                s = {t, n, this};
            }
        }
    }
};
