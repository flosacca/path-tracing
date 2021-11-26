#pragma once
#include "model.h"
#include "trimat.h"
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

    void find(const Ray& r, Detail& s) const {
        Triangle::Face f;
        bvh.intersect(r, f);
        if (f.t < s.t) {
            Vec n = glm::normalize(f(f.s->n));
            if (im) {
                glm::dvec2 p = f(f.s->q);
                Vec c = im.sample(p.x, p.y);
                s = {f.t, n, c, a.e, a.m};
            } else {
                s = {f.t, n, a.c, a.e, a.m};
            }
        }
    }

private:
    struct Triangle {
        std::array<Vec, 3> p;
        std::array<Vec, 3> n;
        std::array<glm::dvec2, 3> q;
        TriMat m;

        struct Face {
            double t = INF;
            double b1 = 0;
            double b2 = 0;
            const Triangle* s = nullptr;

            template <typename T>
            T operator()(const std::array<T, 3>& e) {
                return e[0] + b1 * e[1] + b2 * e[2];
            }
        };

        Box box() const {
            return Box::of(p[0], p[0] + p[1], p[0] + p[2]);
        }

        void intersect(const Ray& r, Face& f) const {
            Vec s;
            if (m.solve(r.o, r.d, s) && s.x < f.t) {
                f = {s.x, s.y, s.z, this};
            }
        }
    };

    BVH<Triangle> bvh;
    Image im;

    Mesh(const std::vector<Vec>& vertices,
         const std::vector<Index>& indices,
         const Image& texture,
         const std::vector<glm::dvec2>& uvs,
         const Vec& color,
         Material material,
         const Vec& emission)
        : Model(color, emission, material), im(texture) {
        auto f = [] (auto&& c, auto&& e) {
            auto i = e[0], j = e[1], k = e[2];
            using T = meta::decay<decltype(c[i])>;
            return std::array<T, 3> {c[i], c[j] - c[i], c[k] - c[i]};
        };
        struct Pair {
            std::array<Vec, 3> p;
            Vec n;
        };
        std::vector<Vec> normals(vertices.size());
        std::vector<Pair> t;
        for (auto&& e : indices) {
            auto p = f(vertices, e);
            Vec n = glm::cross(p[1], p[2]);
            for (int j = 0; j < 3; ++j) {
                normals[e[j]] += n;
            }
            t.push_back({p, n});
        }
        std::vector<Triangle> triangles;
        for (size_t i = 0; i != indices.size(); ++i) {
            if (cmp::zero(glm::length(t[i].n))) {
                continue;
            }
            auto&& e = indices[i];
            auto n = f(normals, e);
            auto q = meta::when(im, [&] { return f(uvs, e); });
            auto& p = t[i].p;
            TriMat m(p[0], p[1], p[2]);
            triangles.push_back({t[i].p, n, q, m});
        }
        bvh.build(triangles);
    }
};
