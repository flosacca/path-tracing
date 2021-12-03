#pragma once
#include "std.h"

namespace meta {
template <typename...>
using always_void = void;

template <typename...>
constexpr bool always_false = false;

template <typename T>
struct type_t {
    using type = T;
};

template <typename T>
constexpr type_t<T> type{};

template <typename T>
constexpr type_t<decay<T>> type_of(T&&) {
    return {};
}

template <typename S>
constexpr int index_impl() {
    return -1;
}

template <typename S, typename T, typename... Ts>
constexpr int index_impl() {
    if (is_same<S, T>) {
        return 0;
    }
    constexpr int i = index_impl<S, Ts...>();
    return i == -1 ? -1 : i + 1;
}

template <typename S, typename... Ts>
constexpr int index = index_impl<S, Ts...>();
} // namespace meta
