#include <happly.h>
#include "ply.h"

struct PlyWrapper {
    happly::PLYData ply;
    happly::Element& vertex_el;

    PlyWrapper(const std::string& path) :
        ply(path), vertex_el(ply.getElement("vertex")) {}
};

inline static PlyWrapper* cast(void* p) {
    return static_cast<PlyWrapper*>(p);
}

inline static PlyWrapper& get(const std::shared_ptr<void>& s) {
    return *cast(s.get());
}

Ply::Ply(const std::string& path) :
    s(new PlyWrapper(path), [] (void* p) { delete cast(p); }) {}

std::vector<float> Ply::getVertexProperty(const std::string& name) const {
    return get(s).vertex_el.getProperty<float>(name);
}

std::vector<std::vector<int>> Ply::getFaceIndices() const {
    return get(s).ply.getFaceIndices<int>();
}
