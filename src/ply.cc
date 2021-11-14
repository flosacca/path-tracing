#include <happly.h>
#include "ply.h"

void loadPly(const std::string& filename, std::vector<Vec>& vertices, std::vector<Mesh::Index>& indices) {
    happly::PLYData plyIn(filename);
    for (auto&& p : plyIn.getVertexPositions()) {
        vertices.push_back(Vec(p[0], p[1], p[2]));
    }
    for (auto&& s : plyIn.getFaceIndices<int>()) {
        indices.push_back({s[0], s[1], s[2]});
    }
}
