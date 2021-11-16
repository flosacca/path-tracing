#include <happly.h>
#include "scene.h"

Scene Scene::load(const Yaml& models) {
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

void Scene::loadPly(const std::string& path, std::vector<Vec>& a, std::vector<Mesh::Index>& b) {
    happly::PLYData f(path);
    for (auto&& p : f.getVertexPositions()) {
        a.push_back({p[0], p[1], p[2]});
    }
    for (auto&& s : f.getFaceIndices<int>()) {
        b.push_back({s[0], s[1], s[2]});
    }
}
