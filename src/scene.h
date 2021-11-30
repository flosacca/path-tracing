#pragma once
#include "mesh.h"
#include "env.h"

struct Scene {
    using ModelEnum = TypeEnum<Sphere, Mesh>;
    using AtomEnum = TypeEnum<Sphere, Mesh::Triangle>;

    std::vector<Plane> planes;
    std::vector<ModelEnum> models;
    std::vector<AtomEnum> atoms;
    BVH<AtomEnum> bvh;

    template <typename T, typename... Args>
    void add(Args&&... args) {
        models.push_back(T(std::forward<Args>(args)...));
    }

    void build() {
        for (auto& p : models) {
            if (p.is<Sphere>()) {
                atoms.push_back(p.as<Sphere>());
            } else if (p.is<Mesh>()) {
                auto& m = p.as<Mesh>();
                for (auto& t : m.triangles) {
                    t.self = &m;
                    atoms.push_back(t);
                }
            }
        }
        bvh.build(atoms);
    }

    void find(const Ray& r, Model::Detail& s) const {
        for (const auto& m : planes) {
            m.find(r, s);
        }
        TypeEnum<Mesh::Triangle::Face, Sphere::Result, Model::Result> f = Model::Result {INF};
        bvh.intersect(r, f);
        f.then([&] (auto& a) {
            a.update(s);
        });
    }

    static Scene load(const Env& models);
};

template <>
struct BVHTrait<Scene::AtomEnum> {
    static Box box(const Scene::AtomEnum& self) {
        return self.then([] (const auto& a) {
            return a.box();
        });
    }

    template <typename S>
    static void intersect(const Scene::AtomEnum& self, const Ray& r, S& s) {
        return self.then([&] (const auto& a) {
            return a.intersect(r, s);
        });
    }
};
