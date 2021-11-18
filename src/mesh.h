#pragma once
#include "model.h"
#include "bvh.h"
#include "image.h"

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

    void find(const Ray& r, Detail& d) const override {
        Local s {INF};
        bvh.intersect(r, s);
        if (s.t < d.i.t) {
            d.i = {s.t, s.p->n};
            if (b.empty()) {
                d.v = meta;
            } else {
                auto&& e = b[s.p - a.data()];
                glm::dvec2 p = e[0] + s.l1 * e[1] + s.l2 * e[2];
                Vec c = im.sample(p.x, p.y);
                d.v = {c, meta.e, meta.m};
            }
        }
    }

private:
    struct Triangle;

    struct Local {
        double t, l1, l2;
        const Triangle* p;
    };

    struct Triangle {
        Vec p[3];
        Vec n;

        void intersect(const Ray& r, Local& s) const {
            Vec e1 = p[1] - p[0];
            Vec e2 = p[2] - p[0];
            Vec v1 = glm::cross(r.d, e2);
            double d = glm::dot(v1, e1);
            if (num::zero(d)) {
                return;
            }
            double c = 1 / d;
            Vec u = r.o - p[0];
            double l1 = c * glm::dot(v1, u);
            if (!num::inclusive(l1, 0, 1)) {
                return;
            }
            Vec v2 = glm::cross(u, e1);
            double l2 = c * glm::dot(v2, r.d);
            if (!num::inclusive(l2, 0, 1 - l1)) {
                return;
            }
            double t = c * glm::dot(v2, e2);
            if (num::greater(t, 0) && t < s.t) {
                s = {t, l1, l2, this};
            }
        }

        Box box() const {
            return Box::of(p[0], p[1], p[2]);
        }

        static double area(const Vec& a, const Vec& b, const Vec& c) {
            return glm::length(glm::cross(a - c, b - c));
        }
    };

    std::vector<Triangle> a;
    BVH<Triangle> bvh;
    Image im;
    std::vector<std::array<glm::dvec2, 3>> b;

    Mesh(const std::vector<Vec>& vertices,
         const std::vector<Index>& indices,
         const Image& texture,
         const std::vector<glm::dvec2>& uvs,
         const Vec& color,
         Material material,
         const Vec& emission)
        : Model(color, emission, material),
          im(texture) {
        auto&& p = vertices;
        auto&& q = uvs;
        for (const auto& e : indices) {
            int i = e[0];
            int j = e[1];
            int k = e[2];
            Vec n = glm::normalize(glm::cross(p[j] - p[i], p[k] - p[i]));
            a.push_back({p[i], p[j], p[k], n});
            if (q.size()) {
                b.push_back({q[i], q[j] - q[i], q[k] - q[i]});
            }
        }
        bvh.build(a);
    }
};
