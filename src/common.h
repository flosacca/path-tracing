#pragma once
#include "meta/all.h"
#include "num/all.h"
#include "vec.h"

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
inline Recursive::Y<meta::decay<F>> recursive(F&& f) {
    return {std::forward<F>(f)};
}

struct Destructor {
    template <typename T>
    void operator()(T&& v) const {
        using V = meta::decay<T>;
        v.~V();
    }
};

constexpr Destructor destructor {};
} // namespace fun

template <typename T, intmax_t D>
using Comparator = num::Comparator<T, std::integral_constant<intmax_t, D>>;

using cmp = Comparator<float, 10000>;

constexpr float T_MIN = 0.01;

struct Ray {
    Vec o, d;

    Ray(const Vec& o, const Vec& d) :
        o(o), d(d) {}

    Vec operator()(float t) const {
        return o + t * d;
    }
};
