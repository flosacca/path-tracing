#pragma once
#include <new>
#include "common.h"

struct Material {
    struct Visitor;

    struct Base {
        virtual Vec accept(Visitor&) const = 0;
    };

    struct Diffuse final : Base {
        Vec accept(Visitor& v) const final {
            return v.visit(*this);
        }
    };

    struct Specular final : Base {
        Vec accept(Visitor& v) const final {
            return v.visit(*this);
        }
    };

    struct Refraction final : Base {
        double n;

        Refraction(double n = 1.5) : n(n) {}

        Vec accept(Visitor& v) const final {
            return v.visit(*this);
        }
    };

    struct Visitor {
        Vec visit(const Base& b) {
            return b.accept(*this);
        }

        virtual Vec visit(const Diffuse&) = 0;

        virtual Vec visit(const Specular&) = 0;

        virtual Vec visit(const Refraction&) = 0;
    };

    union Storage {
        Diffuse d;
        Specular s;
        Refraction r;
    };

    char p[sizeof(Storage)];

    Vec radiance(Visitor& v) const {
        return reinterpret_cast<const Base*>(p)->accept(v);
    }

    template <typename... Args>
    static Material diffuse(Args&&... args) {
        Material m;
        new (m.p) Diffuse(std::forward<Args>(args)...);
        return m;
    }

    template <typename... Args>
    static Material specular(Args&&... args) {
        Material m;
        new (m.p) Specular(std::forward<Args>(args)...);
        return m;
    }

    template <typename... Args>
    static Material refraction(Args&&... args) {
        Material m;
        new (m.p) Refraction(std::forward<Args>(args)...);
        return m;
    }
};
