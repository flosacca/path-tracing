#pragma once
#include "common.h"

template <typename... Ts>
class TypeEnum {
public:
    template <typename T, typename... Args>
    static TypeEnum of(Args&&... args) {
        TypeEnum r;
        r.tag = tag_of<T>();
        new (&r.storage) T {std::forward<Args>(args)...};
        return r;
    }

    template <typename F>
    auto then(F&& f) const {
        return mp::select<Ts...>(tag, [&] (auto i) {
            return f(as<typename decltype(i)::type>());
        });
    }

    template <typename T>
    const T& as() const {
        return *reinterpret_cast<const T*>(storage);
    }

    template <typename T>
    bool is() const {
        return tag == tag_of<T>();
    }

    size_t index() const {
        return tag;
    }

    template <typename T>
    constexpr static size_t tag_of() {
        return mp::index<T, Ts...>();
    }

private:
    size_t tag = -1;
    uint8_t storage[mp::max_size<Ts...>()];
};
