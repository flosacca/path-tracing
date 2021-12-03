#pragma once
#include "material.h"

class Model {
public:
    struct Detail {
        float t = INF;
        Vec n;
        Vec c;
        Vec e;
        Material m;
    };

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
    float r;
    Vec o;

public:
    Sphere(float r, const Vec& o, const Vec& c,
        Material m = Material::diffuse(), const Vec& e = Vec(0)) :
        Model(c, e, m), r(r), o(o) {}

    void find(const Ray& ray, Detail& s) const {
        Vec v = o - ray.o;
        float b = glm::dot(v, ray.d);
        float d2 = r * r - (glm::dot(v, v) - b * b);
        if (cmp::greater(d2, 0)) {
            float d = glm::sqrt(d2);
            float t1 = b + d;
            if (cmp::greater(t1, 0)) {
                float t2 = b - d;
                float t = cmp::greater(t2, 0) ? t2 : t1;
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

    void find(const Ray& r, Detail& s) const {
        float d = glm::dot(r.d, n);
        if (cmp::nonzero(d)) {
            float t = glm::dot(p - r.o, n) / d;
            if (cmp::greater(t, 0) && t < s.t) {
                s = {t, n, a.c, a.e, a.m};
            }
        }
    }
};
