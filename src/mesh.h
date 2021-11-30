#pragma once
#include "model.h"
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

    struct Triangle {
        std::array<Vec, 3> p;
        std::array<Vec, 3> n;
        std::array<glm::dvec2, 3> q;
        const Mesh* self;

        struct Face {
            double t;
            double b1;
            double b2;
            const Triangle* s;

            template <typename T>
            T operator()(const std::array<T, 3>& e) const {
                return e[0] + b1 * e[1] + b2 * e[2];
            }

            void update(Detail& d) const {
                if (t < d.t) {
                    s->self->update(*this, d);
                }
            }
        };

        Box box() const {
            return Box::of(p[0], p[0] + p[1], p[0] + p[2]);
        }

        template <typename T>
        void intersect(const Ray& r, T& f) const {
            Vec v1 = glm::cross(r.d, p[2]);
            double d = glm::dot(v1, p[1]);
            if (cmp::zero(d)) {
                return;
            }
            double c = 1 / d;
            Vec u = r.o - p[0];
            double b1 = c * glm::dot(v1, u);
            if (!cmp::inclusive(b1, 0, 1)) {
                return;
            }
            Vec v2 = glm::cross(u, p[1]);
            double b2 = c * glm::dot(v2, r.d);
            if (!cmp::inclusive(b2, 0, 1 - b1)) {
                return;
            }
            double t = c * glm::dot(v2, p[2]);
            if (cmp::greater(t, 0) && t < static_cast<const Model::Result&>(f).t) {
                f = Face {t, b1, b2, this};
            }
        }
    };

    std::vector<Triangle> triangles;
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
        for (size_t i = 0; i != indices.size(); ++i) {
            if (cmp::zero(glm::length(t[i].n))) {
                continue;
            }
            auto&& e = indices[i];
            auto n = f(normals, e);
            auto q = meta::when(im, [&] { return f(uvs, e); });
            triangles.push_back({t[i].p, n, q});
        }
    }

    void update(const Triangle::Face& f, Detail& s) const {
        Vec n = glm::normalize(f(f.s->n));
        if (im) {
            glm::dvec2 p = f(f.s->q);
            Vec c = im.sample(p.x, p.y);
            s = {f.t, n, c, a.e, a.m};
        } else {
            s = {f.t, n, a.c, a.e, a.m};
        }
    }
};
