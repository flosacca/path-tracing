#pragma once
#include "mesh.h"
#include "env.h"

struct Scene {
    std::vector<std::shared_ptr<Model>> models;

    Scene() = default;

    Scene(std::initializer_list<Model*> a) :
        models(a.begin(), a.end()) {}

    void find(const Ray& r, Model::Detail& s) const {
        for (const auto& p : models) {
            p->find(r, s);
        }
    }

    template <typename T, typename... Args>
    void add(Args&&... args) {
        models.push_back(std::make_shared<T>(std::forward<Args>(args)...));
    }

    static Scene load(const Env& models);
};
