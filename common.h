#pragma once
#include <cstdio>
#include <limits>

using Vec = glm::dvec3;

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double PI = glm::pi<double>();

constexpr double EPS = 1e-4;

inline void print(const Vec& a) {
    printf("(%.3f, %.3f, %.3f)\n", a.x, a.y, a.z);
}

struct Ray {
    Vec o, d;

    Ray(const Vec& o, const Vec& d) :
        o(o), d(d) {}

    Vec operator()(double t) const {
        return o + t * d;
    }
};
