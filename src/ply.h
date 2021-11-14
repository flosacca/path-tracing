#pragma once
#include "mesh.h"

void loadPly(const std::string& filename, std::vector<Vec>& vertices, std::vector<Mesh::Index>& indices);
