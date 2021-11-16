#include <happly.h>
#include "scene.h"

void Scene::loadPly(const std::string& path, std::vector<Vec>& a, std::vector<Mesh::Index>& b) {
    happly::PLYData f(path);
    for (auto&& p : f.getVertexPositions()) {
        a.push_back({p[0], p[1], p[2]});
    }
    for (auto&& s : f.getFaceIndices<int>()) {
        b.push_back({s[0], s[1], s[2]});
    }
}
