#pragma once

using Vec = glm::dvec3;

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double PI = glm::pi<double>();

namespace UnitVec {
inline Vec from_z(double phi, double z) {
    double rho = glm::sqrt(1 - z * z);
    double x = rho * glm::cos(phi);
    double y = rho * glm::sin(phi);
    return {x, y, z};
}

inline Vec from_theta(double phi, double theta) {
    return from_z(phi, glm::cos(theta));
}

inline Vec ortho(const Vec& w) {
    double theta = glm::acos(w.z);
    double phi = glm::atan(w.y, w.x);
    return from_theta(phi, glm::mod(theta + PI / 2, PI));
}

inline Vec from_rho(double phi, double rho) {
    double x = rho * glm::cos(phi);
    double y = rho * glm::sin(phi);
    double z = glm::sqrt(1 - rho * rho);
    return {x, y, z};
}

inline Vec from_rho(double phi, double rho, const Vec& i, const Vec& j, const Vec& k) {
    Vec p = from_rho(phi, rho);
    return p.x * i + p.y * j + p.z * k;
}

inline Vec from_rho(double phi, double rho, const Vec& k) {
    Vec i = ortho(k);
    Vec j = glm::cross(k, i);
    return from_rho(phi, rho, i, j, k);
}
} // namespace UnitVec
