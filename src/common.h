#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

using Vec = glm::dvec3;

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double PI = glm::pi<double>();

namespace num {
    constexpr double EPS = 1e-4;

    inline bool less(double x, double y) {
        return x < y - EPS;
    }

    inline bool lessEqual(double x, double y) {
        return x < y + EPS;
    }

    inline bool greater(double x, double y) {
        return less(y, x);
    }

    inline bool greaterEqual(double x, double y) {
        return lessEqual(y, x);
    }

    inline bool inclusive(double x, double a, double b) {
        return a - EPS < x && x < b + EPS;
    }

    inline bool exclusive(double x, double a, double b) {
        return a + EPS < x & x < b - EPS;
    }

    inline bool equal(double x, double y) {
        return inclusive(x, y, y);
    }

    inline bool zero(double x) {
        return inclusive(x, 0, 0);
    }

    inline bool nonzero(double x) {
        return !inclusive(x, 0, 0);
    }

    inline int sign(double x) {
        return x < -EPS ? -1 : x > EPS;
    }
}

struct Ray {
    Vec o, d;

    Ray(const Vec& o, const Vec& d) :
        o(o), d(d) {}

    Vec operator()(double t) const {
        return o + t * d;
    }
};

enum struct Material {
    DIFFUSE,
    SPECULAR,
    REFRACTION,
};

struct Model;

struct Intersection {
    double t;
    Vec n;
    const Model* m;

    Intersection(double t, const Vec& n, const Model* m) :
        t(t), n(n), m(m) {}

    Intersection() : t(INF), n(0), m(nullptr) {}
};

inline bool operator<(const Intersection& a, const Intersection& b) {
    return a.t < b.t;
}

struct Recursive {
    template <typename F>
    struct Y {
        F f;

        template <typename... Args>
        decltype(auto) operator()(Args&&... args) const {
            return f(std::cref(*this), std::forward<Args>(args)...);
        }
    };

    template <typename F>
    static Y<std::decay_t<F>> of(F&& f) {
        return {f};
    }
};
