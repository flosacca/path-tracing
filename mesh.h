#pragma once
#include "model.h"
#include "bvh.h"

#ifndef RAY_TRIANGLE_OPTION
#define RAY_TRIANGLE_OPTION 1
#endif

struct Triangle {
    Vec p[3];
    Vec n;

    Triangle(const Vec& a, const Vec& b, const Vec& c) :
        p {a, b, c},
        n(glm::normalize(glm::cross(a - c, b - c))) {}

    bool intersect(const Ray& r, double& t) const {
#if RAY_TRIANGLE_OPTION == 0
        Vec q;
        if (!glm::intersectLineTriangle(r.o, r.d, p[0], p[1], p[2], q)) {
            return false;
        }
        t = q.x;
#elif RAY_TRIANGLE_OPTION == 1
        Vec e1 = p[1] - p[0];
        Vec e2 = p[2] - p[0];
        Vec v1 = glm::cross(r.d, e2);
        double d = glm::dot(v1, e1);
        if (num::zero(d)) {
            return false;
        }
        double s = 1 / d;
        Vec u = r.o - p[0];
        double l1 = s * glm::dot(v1, u);
        if (!num::exclusive(l1, 0, 1)) {
            return false;
        }
        Vec v2 = glm::cross(u, e1);
        double l2 = s * glm::dot(v2, r.d);
        if (!num::exclusive(l2, 0, 1 - l1)) {
            return false;
        }
        t = s * glm::dot(v2, e2);
#elif RAY_TRIANGLE_OPTION == 2
        double d = glm::dot(r.d, n);
        if (num::zero(d)) {
            return false;
        }
        t = glm::dot(p[0] - r.o, n) / d;
        Vec q = r(t);
        double s1 = area(q, p[0], p[1]);
        double s2 = area(q, p[1], p[2]);
        double s3 = area(q, p[2], p[0]);
        if (!num::equal(area(p[0], p[1], p[2]), s1 + s2 + s3)) {
            return false;
        }
#endif
        return num::greater(t, 0);
    }

    void intersect(const Ray& r, Intersection& s) const {
        double t;
        if (intersect(r, t) && t < s.t) {
            s.t = t;
            s.n = n;
        }
    }

    Box box() const {
        return Box::of(p[0], p[1], p[2]);
    }

    static double area(const Vec& a, const Vec& b, const Vec& c) {
        return glm::length(glm::cross(a - c, b - c));
    }
};

struct Mesh : Model {
    using Index = std::array<int, 3>;

    std::vector<Triangle> t;
    BVH<Triangle> bvh;

    Mesh(const std::vector<Vec>& vertices,
         const std::vector<Index>& indices,
         const Vec& color,
         Material material = Material::DIFFUSE,
         const Vec& emission = Vec(0))
        : Model(emission, color, material) {
        for (const auto& index : indices) {
            const Vec& a = vertices[index[0]];
            const Vec& b = vertices[index[1]];
            const Vec& c = vertices[index[2]];
            t.push_back({a, b, c});
        }
        bvh.build(t.begin(), t.end());
    }

    void intersect(const Ray& r, Intersection& s) const {
        bvh.intersect(r, s);
        if (s.t < INF) {
            s.m = this;
        }
    }

    Intersection find(const Ray& r) const {
        Intersection s;
        intersect(r, s);
        return s;
    }
};
