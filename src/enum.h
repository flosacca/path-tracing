#pragma once
#include "common.h"

template <typename... Ts>
class TypeEnum {
public:
    template <typename T>
    constexpr static int which = meta::index<T, Ts...>;

    TypeEnum() = default;

    TypeEnum(const TypeEnum& rhs) {
        rhs.then([&] (auto&& v) {
            take(v);
        });
    }

    TypeEnum(TypeEnum&& rhs) {
        rhs.then([&] (auto&& v) {
            take(std::move(v));
        });
    }

    template <typename T>
    TypeEnum(T&& v) {
        take(static_cast<T&&>(v));
    }

    TypeEnum& operator=(const TypeEnum& rhs) {
        this->~TypeEnum();
        new (this) TypeEnum(rhs);
        return *this;
    }

    TypeEnum& operator=(TypeEnum&& rhs) {
        this->~TypeEnum();
        new (this) TypeEnum(std::move(rhs));
        return *this;
    }

    template <typename T>
    TypeEnum& operator=(T&& v) {
        this->~TypeEnum();
        new (this) TypeEnum(static_cast<T&&>(v));
        return *this;
    }

    ~TypeEnum() {
        apply(fun::destructor, *this);
    }

    template <typename T, typename... Args>
    static TypeEnum of(Args&&... args) {
        return TypeEnum(meta::type<T>, static_cast<Args&&>(args)...);
    }

    template <typename T, typename... Args>
    void construct(Args&&... args) {
        static_assert(which<T> != -1);
        tag = which<T>;
        new (&storage) T (static_cast<Args&&>(args)...);
    }

    template <typename F>
    auto then(F&& f) {
        return apply(static_cast<F&&>(f), *this);
    }

    template <typename F>
    auto then(F&& f) const {
        return apply(static_cast<F&&>(f), *this);
    }

    template <typename T>
    T& as() {
        return *reinterpret_cast<T*>(&storage);
    }

    template <typename T>
    const T& as() const {
        return *reinterpret_cast<const T*>(&storage);
    }

    template <typename T>
    bool is() const {
        return tag == which<T>;
    }

    int index() const {
        return tag;
    }

private:
    template <typename T, typename... Args>
    TypeEnum(T type, Args&&... args) {
        construct<typename decltype(type)::type>(static_cast<Args&&>(args)...);
    }

    template <typename T>
    void take(T&& v) {
        construct<meta::decay<T>>(static_cast<T&&>(v));
    }

    template <typename F, typename Self>
    static auto apply(F&& f, Self&& self) {
        return meta::select<Ts...>(self.tag, [&] (auto type) {
            return f(self.template as<typename decltype(type)::type>());
        });
    }

    std::aligned_union_t<0, Ts...> storage;
    int tag = -1;
};
