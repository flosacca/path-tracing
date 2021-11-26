#pragma once
#include "material.h"

class Model {
public:
    struct Detail {
        double t = INF;
        Vec n;
        Vec c;
        Vec e;
        Material m;
    };

    virtual void find(const Ray&, Detail&) const = 0;

    virtual ~Model() = default;

protected:
    struct Attr {
        Vec c;
        Vec e;
        Material m;
    };

    Attr a;

    template <typename... Args>
    Model(Args&&... args) :
        a {std::forward<Args>(args)...} {}
};

class Sphere : public Model {
private:
    double r;
    Vec o;

public:
    Sphere(double r, const Vec& o, const Vec& c,
        Material m = Material::diffuse(), const Vec& e = Vec(0)) :
        Model(c, e, m), r(r), o(o) {}

    void find(const Ray& ray, Detail& s) const final {
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
                    s = {t, n, a.c, a.e, a.m};
                }
            }
        }
    }
};

class Plane : public Model {
private:
    Vec n, p;

public:
    Plane(const Vec& n, const Vec& p, const Vec& c,
        Material m = Material::diffuse(), const Vec& e = Vec(0)) :
        Model(c, e, m), n(n), p(p) {}

    void find(const Ray& r, Detail& s) const final {
        double d = glm::dot(r.d, n);
        if (num::nonzero(d)) {
            double t = glm::dot(p - r.o, n) / d;
            if (num::greater(t, 0) && t < s.t) {
                s = {t, n, a.c, a.e, a.m};
            }
        }
    }
};
