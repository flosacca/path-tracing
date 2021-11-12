#pragma once
#include "common.h"

enum struct Material {
    DIFFUSE,
    SPECULAR,
    REFRACTION,
};

struct Model;

struct Intersection {
    double t;
    Vec n;
    const Model* m;

    Intersection(double t, const Vec& n, const Model* m) :
        t(t), n(n), m(m) {}

    Intersection() : t(INF), n(0), m(nullptr) {}
};

inline bool operator<(const Intersection& a, const Intersection& b) {
    return a.t < b.t;
}

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
        Material m = Material::DIFFUSE, const Vec& e = Vec(0)) :
        Model(e, c, m), r(r), o(o) {}

    Intersection find(const Ray& ray) const {
        Vec v = o - ray.o;
        double b = glm::dot(v, ray.d);
        double d2 = r * r - (glm::dot(v, v) - b * b);
        if (d2 > EPS) {
            double d = glm::sqrt(d2);
            if (b + d > EPS) {
                double t = b + d;
                if (b - d > EPS) {
                    t = b - d;
                }
                return {t, glm::normalize(ray(t) - o), this};
            }
        }
        return {};
    }
};

struct Plane : Model {
    Vec n, p;

    Plane(const Vec& n, const Vec& p, const Vec& c,
        Material m = Material::DIFFUSE, const Vec& e = Vec(0)) :
        Model(e, c, m), n(n), p(p) {}

    Intersection find(const Ray& r) const {
        double w = glm::dot(r.d, n);
        if (glm::abs(w) > EPS) {
            double t = glm::dot(p - r.o, n) / w;
            if (t > EPS) {
                return {t, n, this};
            }
        }
        return {};
    }
};
