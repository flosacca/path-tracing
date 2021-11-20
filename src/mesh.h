#pragma once
#include "model.h"
#include "bvh.h"
#include "image.h"

#ifndef RAY_TRIANGLE_OPTION
#define RAY_TRIANGLE_OPTION 1
#endif

class Mesh : public Model {
public:
    using Index = std::array<int, 3>;

    Mesh(const std::vector<Vec>& vertices,
         const std::vector<Index>& indices,
         const Vec& color,
         Material material = Material::diffuse(),
         const Vec& emission = Vec(0))
        : Mesh(vertices, indices, {}, {}, color, material, emission) {}

    Mesh(const std::vector<Vec>& vertices,
         const std::vector<Index>& indices,
         const Image& texture,
         const std::vector<glm::dvec2>& uvs,
         Material material = Material::diffuse(),
         const Vec& emission = Vec(0))
        : Mesh(vertices, indices, texture, uvs, {}, material, emission) {}

    void intersect(const Ray& r, Intersection& s) const {
        bvh.intersect(r, s);
        if (s.t < INF) {
            s.m = this;
        }
    }

    Intersection find(const Ray& r) const override {
        Intersection s;
        intersect(r, s);
        return s;
    }

private:
    struct Triangle {
        Vec p[3];
        Vec n;

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
            if (!num::inclusive(l1, 0, 1)) {
                return false;
            }
            Vec v2 = glm::cross(u, e1);
            double l2 = s * glm::dot(v2, r.d);
            if (!num::inclusive(l2, 0, 1 - l1)) {
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

    std::vector<Vec> vertices;
    std::vector<Index> indices;
    std::vector<Triangle> triangles;
    BVH<Triangle> bvh;
    Image texture;
    std::vector<glm::dvec2> uvs;

    Mesh(const std::vector<Vec>& vertices,
         const std::vector<Index>& indices,
         const Image& texture,
         const std::vector<glm::dvec2>& uvs,
         const Vec& color,
         Material material,
         const Vec& emission)
        : Model(color, emission, material),
          vertices(vertices),
          indices(indices),
          texture(texture),
          uvs(uvs) {
        for (const auto& index : indices) {
            const Vec& a = vertices[index[0]];
            const Vec& b = vertices[index[1]];
            const Vec& c = vertices[index[2]];
            triangles.push_back({a, b, c, glm::normalize(glm::cross(b - a, c - a))});
        }
        bvh.build(triangles);
    }
};
