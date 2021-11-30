#pragma once
#include "material.h"
#include "bvh.h"

class Model {
public:
    struct Detail {
        double t = INF;
        Vec n;
        Vec c;
        Vec e;
        Material m;
    };

    struct Result {
        double t;

        void update(Detail&) {}
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
    double r;
    Vec o;
    Box m_box;

public:
    struct Result {
        double t;
        Vec n;
        const Sphere* s;

        void update(Detail& d) const {
            if (t < d.t) {
                s->update(*this, d);
            }
        }
    };

    Sphere(double r, const Vec& o, const Vec& c,
        Material m = Material::diffuse(), const Vec& e = Vec(0)) :
        Model(c, e, m), r(r), o(o), m_box(o - r, o + r) {}

    Box box() const {
        return m_box;
    }

    template <typename T>
    void intersect(const Ray& ray, T& f) const {
        Vec v = o - ray.o;
        double b = glm::dot(v, ray.d);
        double d2 = r * r - (glm::dot(v, v) - b * b);
        if (cmp::greater(d2, 0)) {
            double d = glm::sqrt(d2);
            double t1 = b + d;
            if (cmp::greater(t1, 0)) {
                double t2 = b - d;
                double t = cmp::greater(t2, 0) ? t2 : t1;
                if (t < static_cast<const Model::Result&>(f).t) {
                    Vec n = glm::normalize(ray(t) - o);
                    f = Result {t, n, this};
                }
            }
        }
    }

    void update(const Result& f, Detail& s) const {
        s = {f.t, f.n, a.c, a.e, a.m};
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
        double d = glm::dot(r.d, n);
        if (cmp::nonzero(d)) {
            double t = glm::dot(p - r.o, n) / d;
            if (cmp::greater(t, 0) && t < s.t) {
                s = {t, n, a.c, a.e, a.m};
            }
        }
    }
};
