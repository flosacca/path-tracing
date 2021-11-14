#pragma once
#include "model.h"

struct Scene {
    std::vector<std::shared_ptr<Model>> models;

    Scene(std::initializer_list<Model*> a) :
        models(a.begin(), a.end()) {}

    Intersection find(const Ray& r) const {
        Intersection s;
        for (const auto& p : models) {
            s = std::min(s, p->find(r));
        }
        return s;
    }
};
