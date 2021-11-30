#pragma once

namespace num {
template <typename T>
constexpr T eps();

template <typename T>
struct Comparator {
    constexpr static T EPS = num::eps<T>();

    constexpr static bool less(T x, T y) {
        return x < y - EPS;
    }

    constexpr static bool lessEqual(T x, T y) {
        return x < y + EPS;
    }

    constexpr static bool greater(T x, T y) {
        return less(y, x);
    }

    constexpr static bool greaterEqual(T x, T y) {
        return lessEqual(y, x);
    }

    constexpr static bool inclusive(T x, T a, T b) {
        return a - EPS < x && x < b + EPS;
    }

    constexpr static bool exclusive(T x, T a, T b) {
        return a + EPS < x & x < b - EPS;
    }

    constexpr static bool equal(T x, T y) {
        return inclusive(x, y, y);
    }

    constexpr static bool zero(T x) {
        return inclusive(x, 0, 0);
    }

    constexpr static bool nonzero(T x) {
        return !inclusive(x, 0, 0);
    }

    constexpr static int sign(T x) {
        return x < -EPS ? -1 : x > EPS;
    }
};
} // namespace num
