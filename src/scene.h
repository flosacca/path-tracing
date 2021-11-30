#pragma once
#include "mesh.h"
#include "env.h"

struct Scene {
    using Poly = TypeEnum<Plane, Sphere, Mesh>;

    std::vector<Poly> models;

    void find(const Ray& r, Model::Detail& s) const {
        for (const auto& p : models) {
            p.then([&] (const auto& m) {
                m.find(r, s);
            });
        }
    }

    template <typename T, typename... Args>
    void add(Args&&... args) {
        models.push_back(T(std::forward<Args>(args)...));
    }

    static Scene load(const Env& models);
};
