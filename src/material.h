#pragma once
#include <new>
#include "common.h"

class Material {
public:
    struct Diffuse {};

    struct Specular {};

    struct Refraction {
        double n = 1.5;
    };

    struct Visitor {
        virtual Vec visit(const Diffuse&) = 0;

        virtual Vec visit(const Specular&) = 0;

        virtual Vec visit(const Refraction&) = 0;
    };

    Vec radiance(Visitor& v) const {
        return f(*this, v);
    }

    template <typename T, typename... Args>
    static Material make(Args&&... args) {
        Material m;
        m.f = [] (const Material& self, Visitor& v) {
            return v.visit(*reinterpret_cast<const T*>(&self.p));
        };
        new (&m.p) T {std::forward<Args>(args)...};
        return m;
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
    static Material refraction(Args&&... args) {
        return make<Refraction>(std::forward<Args>(args)...);
    }

private:
    constexpr static int N = num::max_size<Diffuse, Specular, Refraction>();

    Vec (*f)(const Material&, Visitor&);
    char p[N];
};
