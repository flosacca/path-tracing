#include "ext/ply.h"
#include "scene.h"

Scene Scene::load(const Env& models) {
    Scene scene;
    auto v0 = env::accessor(Vec(0));
    auto s0 = env::accessor(0.0);

    for (const Env& t : models) {
        auto v = v0(t);
        auto q = env::accessor<std::string>()(t);
        auto type = q("type");

        Vec color = v("color");
        Vec emission = v("emission");
        Material material = env::fetch(t["material"], Material::diffuse());

        if (type == "plane") {
            Vec normal = v("normal");
            Vec point = v("point");
            scene.add<Plane>(normal, point, color, material, emission);

        } else if (type == "sphere") {
            double radius = s0(t)("radius");
            Vec center = v("center");
            scene.add<Sphere>(radius, center, color, material, emission);

        } else if (type == "mesh") {
            Ply ply(q("model"));
            auto prop_x = ply.getVertexProperty("x");
            auto prop_y = ply.getVertexProperty("y");
            auto prop_z = ply.getVertexProperty("z");

            std::vector<Vec> vertices;
            std::vector<Mesh::Index> indices;
            for (size_t i = 0; i != prop_x.size(); ++i) {
                vertices.push_back({prop_x[i], prop_y[i], prop_z[i]});
            }
            for (auto&& e : ply.getFaceIndices()) {
                indices.push_back({ e[0], e[1], e[2] });
            }

            Vec s = env::fetch(t["scale"], Vec(1));
            Vec w = v("translate");
            if (auto r = t["rotate"]) {
                glm::dmat3 f = glm::rotate(glm::radians(s0(r)(0)), v0(r)(1));
                f *= glm::dmat3(glm::scale(s));
                for (Vec& p : vertices) {
                    p = f * p + w;
                }
            } else {
                for (Vec& p : vertices) {
                    p = s * p + w;
                }
            }

            auto im_path = q("texture");
            if (im_path.size()) {
                auto prop_u = ply.getVertexProperty("u");
                auto prop_v = ply.getVertexProperty("v");
                std::vector<glm::dvec2> uvs;
                for (size_t i = 0; i != prop_x.size(); ++i) {
                    uvs.push_back({prop_u[i], prop_v[i]});
                }
                Image texture = Image::load(im_path);
                scene.add<Mesh>(vertices, indices, texture, uvs, material, emission);
            } else {
                scene.add<Mesh>(vertices, indices, color, material, emission);
            }
        }
    }

    return scene;
}
