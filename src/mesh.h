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

    void find(const Ray& r, Detail& s) const final {
        bvh.intersect(r, s);
    }

private:
    struct Triangle {
        Mesh& self;
        std::array<Vec, 3> p;
        Vec n;
        std::array<glm::dvec2, 3> q;

        void intersect(const Ray& r, Detail& s) const {
            Vec v1 = glm::cross(r.d, p[2]);
            double d = glm::dot(v1, p[1]);
            if (num::zero(d)) {
                return;
            }
            double c = 1 / d;
            Vec u = r.o - p[0];
            double l1 = c * glm::dot(v1, u);
            if (!num::inclusive(l1, 0, 1)) {
                return;
            }
            Vec v2 = glm::cross(u, p[1]);
            double l2 = c * glm::dot(v2, r.d);
            if (!num::inclusive(l2, 0, 1 - l1)) {
                return;
            }
            double t = c * glm::dot(v2, p[2]);
            if (num::greater(t, 0) && t < s.t) {
                const Meta& m = self.meta;
                Vec c = m.c;
                if (self.has_texture) {
                    glm::dvec2 p = q[0] + l1 * q[1] + l2 * q[2];
                    c = self.im.sample(p.x, p.y);
                }
                s = {t, n, c, m.e, m.m};
            }
        }

        Box box() const {
            return Box::of(p[0], p[0] + p[1], p[0] + p[2]);
        }

        static double area(const Vec& a, const Vec& b, const Vec& c) {
            return glm::length(glm::cross(a - c, b - c));
        }
    };

    std::vector<Triangle> triangles;
    BVH<Triangle> bvh;
    Image im;
    bool has_texture;

    Mesh(const std::vector<Vec>& vertices,
         const std::vector<Index>& indices,
         const Image& texture,
         const std::vector<glm::dvec2>& uvs,
         const Vec& color,
         Material material,
         const Vec& emission)
        : Model(color, emission, material),
          im(texture),
          has_texture(uvs.size()) {
        auto&& p = vertices;
        auto&& q = uvs;
        for (const auto& e : indices) {
            int i = e[0];
            int j = e[1];
            int k = e[2];
            std::array<Vec, 3> a = {p[i], p[j] - p[i], p[k] - p[i]};
            Vec n = glm::normalize(glm::cross(a[1], a[2]));
            std::array<glm::dvec2, 3> b;
            if (has_texture) {
                b = {q[i], q[j] - q[i], q[k] - q[i]};
            }
            triangles.push_back({*this, a, n, b});
        }
        bvh.build(triangles);
    }
};
