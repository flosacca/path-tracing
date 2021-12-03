#pragma once
#include <memory>
#include <string>
#include <vector>

class Ply {
public:
    Ply(const std::string&);

    std::vector<float> getVertexProperty(const std::string&) const;

    std::vector<std::vector<int>> getFaceIndices() const;

private:
    std::shared_ptr<void> s;
};
