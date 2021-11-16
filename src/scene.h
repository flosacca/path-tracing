#pragma once
#include "mesh.h"
#include "helper.h"

struct Scene {
    std::vector<std::shared_ptr<Model>> models;

    Scene() = default;

    Scene(std::initializer_list<Model*> a) :
        models(a.begin(), a.end()) {}

    Intersection find(const Ray& r) const {
        Intersection s;
        for (const auto& p : models) {
            s = std::min(s, p->find(r));
        }
        return s;
    }

    template <typename T, typename... Args>
    void add(Args&&... args) {
        models.push_back(std::make_shared<T>(std::forward<Args>(args)...));
    }

    static Scene load(const Yaml& models);

    static void loadPly(const std::string&, std::vector<Vec>&, std::vector<Mesh::Index>&);
};
