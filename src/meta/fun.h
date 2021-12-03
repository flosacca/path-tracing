#pragma once
#include <cstdlib>
#include "type.h"

namespace meta {
template <typename T>
constexpr bool has_default = is_default_constructible<T> || is_void<T>;

template <typename T, enable_if<has_default<T>>* = nullptr>
inline T construct() {
    return T();
}

template <typename T, enable_if<!has_default<T>>* = nullptr>
inline T construct() {
    abort();
}

template <typename S, typename F>
inline decltype(auto) select(size_t i, F&& f) {
    if (i == 0) {
        return f(type<S>);
    } else {
        return construct<decltype(f(type<S>))>();
    }
}

template <typename S, typename T, typename... Ts, typename F>
inline decltype(auto) select(size_t i, F&& f) {
    if (i == 0) {
        return f(type<S>);
    } else {
        return meta::select<T, Ts...>(i - 1, static_cast<F&&>(f));
    }
}

template <typename T, typename F>
constexpr auto when(T&& v, F&& f) {
    return (bool) v ? f() : decay<decltype(f())>();
}
} // namespace meta
