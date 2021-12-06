#pragma once
#include "material.h"

using Env = YAML::Node;

namespace env {
    template <typename T>
    struct fetch_impl;

    template <typename T>
    inline T fetch(const Env& e, const T& d) {
        return fetch_impl<T>::call(e, d);
    }

    inline auto bind(const Env& e) {
        return [=] (const auto& i, const auto& d) {
            return fetch(e[i], d);
        };
    }

    template <typename T>
    inline auto accessor(const T& d = T()) {
        return [=] (const Env& e) {
            return [=] (const auto& i) {
                return fetch(e[i], d);
            };
        };
    }

    template <typename T>
    struct fetch_impl {
        static T call(const Env& e, const T& d) {
            return e.as<T>(d);
        }
    };

    template <>
    struct fetch_impl<Vec> {
        static Vec call(const Env& e, const Vec& d) {
            if (!e) {
                return d;
            }
            if (e.IsScalar()) {
                return Vec(fetch(e, 0.0));
            }
            if (e.IsSequence()) {
                auto s = accessor(0.0)(e);
                return Vec(s(0), s(1), s(2));
            }
            return d;
        }
    };

    template <>
    struct fetch_impl<Material> {
        static Material call(const Env& e, const Material& d) {
            if (!e) {
                return d;
            }
            int k = e.IsScalar() | e.IsSequence() << 1;
            if (!k) {
                return d;
            }
            auto t = fetch(k & 1 ? e : e[0], std::string());
            if (t == "diffuse") {
                return Material::diffuse();
            }
            if (t == "specular") {
                return Material::specular();
            }
            if (t == "refractive") {
                constexpr double n = 1.5;
                return Material::refractive(k & 1 ? n : fetch(e[1], n));
            }
            return d;
        }
    };
}
