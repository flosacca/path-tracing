#pragma once
#include "model.h"

class Scene {
private:
    std::vector<std::shared_ptr<Model>> models;

public:
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
