#pragma once

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

using Vec = glm::dvec3;

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double PI = glm::pi<double>();

namespace num {
    namespace eps {
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

    namespace unit {
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
    }

    template <unsigned N, typename T>
    struct pow_t;

    template <unsigned N, typename T>
    inline constexpr T pow(const T& x) {
        return pow_t<N, T>::call(x);
    }

    template <unsigned N, typename T>
    struct pow_t {
        constexpr static T call(const T& x) {
            return pow<N / 2>(x) * pow<N - N / 2>(x);
        }
    };

    template <typename T>
    struct pow_t<1, T> {
        constexpr static T call(const T& x) {
            return x;
        }
    };

    template <typename T>
    struct pow_t<0, T> {
        constexpr static T call(const T& x) {
            return T(1);
        }
    };

    using namespace eps;
}

struct Ray {
    Vec o, d;

    Ray(const Vec& o, const Vec& d) :
        o(o), d(d) {}

    Vec operator()(double t) const {
        return o + t * d;
    }
};

class Model;

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