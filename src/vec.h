#pragma once

using Vec = glm::vec3;

constexpr float INF = std::numeric_limits<float>::infinity();
constexpr float PI = 3.14159265358979323846264338327950288;

namespace UnitVec {
inline Vec from_z(float phi, float z) {
    float rho = glm::sqrt(1 - z * z);
    float x = rho * glm::cos(phi);
    float y = rho * glm::sin(phi);
    return {x, y, z};
}

inline Vec from_theta(float phi, float theta) {
    return from_z(phi, glm::cos(theta));
}

inline Vec ortho(const Vec& w) {
    float theta = glm::acos(w.z);
    float phi = glm::atan(w.y, w.x);
    return from_theta(phi, glm::mod(theta + PI / 2, PI));
}

inline Vec from_rho(float phi, float rho) {
    float x = rho * glm::cos(phi);
    float y = rho * glm::sin(phi);
    float z = glm::sqrt(1 - rho * rho);
    return {x, y, z};
}

inline Vec from_rho(float phi, float rho, const Vec& i, const Vec& j, const Vec& k) {
    Vec p = from_rho(phi, rho);
    return p.x * i + p.y * j + p.z * k;
}

inline Vec from_rho(float phi, float rho, const Vec& k) {
    Vec i = ortho(k);
    Vec j = glm::cross(k, i);
    return from_rho(phi, rho, i, j, k);
}
} // namespace UnitVec
