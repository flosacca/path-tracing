#pragma once
#include "material.h"

namespace hlp {
    template <typename T>
    struct YamlFetcher;

    template <typename T>
    inline T fetch(const Yaml& v, const T& d) {
        return YamlFetcher<T>::call(v, d);
    }

    template <typename T>
    inline auto accessor(const T& d = T()) {
        return [=] (const Yaml& v) {
            return [=] (const auto& i) {
                return fetch(v[i], d);
            };
        };
    }

    template <typename T>
    struct YamlFetcher {
        static T call(const Yaml& v, const T& d) {
            return v.as<T>(d);
        }
    };

    template <>
    struct YamlFetcher<Vec> {
        static Vec call(const Yaml& v, const Vec& d) {
            if (!v) {
                return d;
            }
            if (v.IsScalar()) {
                return Vec(fetch(v, 0.0));
            }
            if (v.IsSequence()) {
                auto s = accessor(0.0)(v);
                return Vec(s(0), s(1), s(2));
            }
            return d;
        }
    };

    template <>
    struct YamlFetcher<Material> {
        static Material call(const Yaml& v, const Material& d) {
            if (!v) {
                return d;
            }
            int k = v.IsScalar() | v.IsSequence() << 1;
            if (!k) {
                return d;
            }
            auto t = fetch(k & 1 ? v : v[0], std::string());
            if (t == "diffuse") {
                return Material::diffuse();
            }
            if (t == "specular") {
                return Material::specular();
            }
            if (t == "refraction") {
                constexpr double n = 1.5;
                return Material::refraction(k & 1 ? n : fetch(v[1], n));
            }
            return d;
        }
    };
}
