#pragma once

namespace fun {
    struct Recursive {
        template <typename F>
        struct Y {
            F f;

            template <typename... Args>
            decltype(auto) operator()(Args&&... args) const {
                return f(std::cref(*this), std::forward<Args>(args)...);
            }
        };
    };

    template <typename F>
    inline Recursive::Y<std::decay_t<F>> recursive(F&& f) {
        return {f};
    }

    template <typename F>
    inline auto eval_if(bool c, F&& f) {
        return c ? f() : std::decay_t<decltype(f())> {};
    }
}

using Vec = glm::dvec3;

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double PI = glm::pi<double>();

namespace num {
    namespace eps {
        constexpr double EPS = 1e-4;

        constexpr bool less(double x, double y) {
            return x < y - EPS;
        }

        constexpr bool lessEqual(double x, double y) {
            return x < y + EPS;
        }

        constexpr bool greater(double x, double y) {
            return less(y, x);
        }

        constexpr bool greaterEqual(double x, double y) {
            return lessEqual(y, x);
        }

        constexpr bool inclusive(double x, double a, double b) {
            return a - EPS < x && x < b + EPS;
        }

        constexpr bool exclusive(double x, double a, double b) {
            return a + EPS < x & x < b - EPS;
        }

        constexpr bool equal(double x, double y) {
            return inclusive(x, y, y);
        }

        constexpr bool zero(double x) {
            return inclusive(x, 0, 0);
        }

        constexpr bool nonzero(double x) {
            return !inclusive(x, 0, 0);
        }

        constexpr int sign(double x) {
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
    constexpr T pow(const T& x) {
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

    template <typename T>
    constexpr const T& min(const T& a, const T& b) {
        return std::min(a, b);
    }

    template <typename T, typename... Args>
    constexpr const T& min(const T& a, const T& b, const Args&... args) {
        return num::min(num::min(a, b), args...);
    }

    template <typename T>
    constexpr const T& max(const T& a, const T& b) {
        return std::max(a, b);
    }

    template <typename T, typename... Args>
    constexpr const T& max(const T& a, const T& b, const Args&... args) {
        return num::max(num::max(a, b), args...);
    }

    using namespace eps;
}

namespace mp {
    template <typename T>
    struct identity {
        using type = T;
    };

    template <typename... Ts>
    constexpr size_t max_size() {
        return num::max(sizeof(Ts)...);
    }

    template <typename S>
    constexpr size_t index() {
        return -1;
    }

    template <typename S, typename T, typename... Ts>
    constexpr size_t index() {
        if (std::is_same<S, T>::value) {
            return 0;
        }
        constexpr size_t i = mp::index<S, Ts...>();
        return i == -1 ? -1 : i + 1;
    }

    template <typename S, typename F>
    inline auto select(size_t i, F&& f) {
        if (i == 0) {
            return f(mp::identity<S>());
        } else {
            throw std::runtime_error("no type of given index");
        }
    }

    template <typename S, typename T, typename... Ts, typename F>
    inline auto select(size_t i, F&& f) {
        if (i == 0) {
            return f(mp::identity<S>());
        } else {
            return mp::select<T, Ts...>(i - 1, std::forward<F>(f));
        }
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
