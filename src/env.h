#pragma once
#include "material.h"

using Env = YAML::Node;

namespace env {
    template <typename T>
    struct fetch_t;

    template <typename T>
    inline T fetch(const Env& e, const T& d) {
        return fetch_t<T>::call(e, d);
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
    struct fetch_t {
        static T call(const Env& e, const T& d) {
            return e.as<T>(d);
        }
    };

    template <>
    struct fetch_t<Vec> {
        static Vec call(const Env& e, const Vec& d) {
            if (!e) {
                return d;
            }
            if (e.IsScalar()) {
                return Vec(fetch(e, 0.0f));
            }
            if (e.IsSequence()) {
                auto s = accessor(0.0f)(e);
                return Vec(s(0), s(1), s(2));
            }
            return d;
        }
    };

    template <>
    struct fetch_t<Material> {
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
                constexpr float n = 1.5f;
                return Material::refractive(k & 1 ? n : fetch(e[1], n));
            }
            return d;
        }
    };
}
