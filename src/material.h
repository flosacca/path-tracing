#pragma once
#include "enum.h"

struct Material {
    struct Diffuse {};

    struct Specular {};

    struct Refractive {
        double n = 1.5;
    };

    template <typename T, typename... Args>
    static Material make(Args&&... args) {
        return {Poly::of<T>(std::forward<Args>(args)...)};
    }

    template <typename... Args>
    static Material diffuse(Args&&... args) {
        return make<Diffuse>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static Material specular(Args&&... args) {
        return make<Specular>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static Material refractive(Args&&... args) {
        return make<Refractive>(std::forward<Args>(args)...);
    }

    template <typename Visitor>
    Vec radiance(Visitor& v) const {
        return p.then([&] (const auto& m) {
            return v.visit(m);
        });
    }

    using Poly = TypeEnum<Diffuse, Specular, Refractive>;

    Poly p;
};
