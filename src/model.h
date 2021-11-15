#pragma once
#include "material.h"

struct Model {
    Vec e, c;
    Material m;

    Model(const Vec& e, const Vec& c, Material m) :
        e(e), c(c), m(m) {}

    virtual Intersection find(const Ray&) const = 0;
};

struct Sphere : Model {
    double r;
    Vec o;

    Sphere(double r, const Vec& o, const Vec& c,
        Material m = Material::diffuse(), const Vec& e = Vec(0)) :
        Model(e, c, m), r(r), o(o) {}

    Intersection find(const Ray& ray) const override {
        Vec v = o - ray.o;
        double b = glm::dot(v, ray.d);
        double d2 = r * r - (glm::dot(v, v) - b * b);
        if (num::greater(d2, 0)) {
            double d = glm::sqrt(d2);
            double t1 = b + d;
            if (num::greater(t1, 0)) {
                double t2 = b - d;
                double t = num::greater(t2, 0) ? t2 : t1;
                Vec n = glm::normalize(ray(t) - o);
                return {t, n, this};
            }
        }
        return {};
    }
};

struct Plane : Model {
    Vec n, p;

    Plane(const Vec& n, const Vec& p, const Vec& c,
        Material m = Material::diffuse(), const Vec& e = Vec(0)) :
        Model(e, c, m), n(n), p(p) {}

    Intersection find(const Ray& r) const override {
        double d = glm::dot(r.d, n);
        if (num::nonzero(d)) {
            double t = glm::dot(p - r.o, n) / d;
            if (num::greater(t, 0)) {
                return {t, n, this};
            }
        }
        return {};
    }
};
