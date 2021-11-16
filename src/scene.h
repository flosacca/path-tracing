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

    static Scene load(const Yaml& models) {
        using namespace helper;
        Scene scene;
        auto v0 = bind<Vec>;
        auto s0 = bind<double>;
        for (const Yaml& t : models) {
            auto v = v0(t);
            auto q = bind<std::string>(t);
            auto k = q("type");
            Vec c = v("color");
            Vec e = v("emission");
            Material m = fetch<Material>(t["material"]);
            if (k == "plane") {
                Vec n = v("normal");
                Vec p = v("point");
                scene.add<Plane>(n, p, c, m, e);
            } else if (k == "sphere") {
                double r = s0(t)("radius");
                Vec p = v("center");
                scene.add<Sphere>(r, p, c, m, e);
            } else if (k == "mesh") {
                std::vector<Vec> a;
                std::vector<Mesh::Index> b;
                loadPly(q("model"), a, b);
                glm::dmat3 f(fetch(t["scale"], 1.0));
                if (auto r = t["rotate"]) {
                    f = glm::rotate(glm::dmat4(f), s0(r)(0), v0(r)(1));
                }
                Vec w = v("translate");
                for (Vec& p : a) {
                    p = f * p + w;
                }
                scene.add<Mesh>(a, b, c, m, e);
            }
        }
        return scene;
    }

    static void loadPly(const std::string&, std::vector<Vec>&, std::vector<Mesh::Index>&);
};
