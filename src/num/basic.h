#pragma once

namespace num {
template <typename T>
constexpr const T& min(const T& a, const T& b) {
    return b < a ? b : a;
}

template <typename T, typename... Args>
constexpr const T& min(const T& a, const T& b, const Args&... args) {
    return num::min(num::min(a, b), args...);
}

template <typename T>
constexpr const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <typename T, typename... Args>
constexpr const T& max(const T& a, const T& b, const Args&... args) {
    return num::max(num::max(a, b), args...);
}

template <unsigned N, typename T>
constexpr T pow(const T& x);

template <unsigned N, typename T>
struct pow_impl {
    constexpr static T call(const T& x) {
        return num::pow<N / 2>(x) * num::pow<N - N / 2>(x);
    }
};

template <typename T>
struct pow_impl<1, T> {
    constexpr static T call(const T& x) {
        return x;
    }
};

template <typename T>
struct pow_impl<0, T> {
    constexpr static T call(const T& x) {
        return T(1);
    }
};

template <unsigned N, typename T>
constexpr T pow(const T& x) {
    return pow_impl<N, T>::call(x);
}
} // namespace num
