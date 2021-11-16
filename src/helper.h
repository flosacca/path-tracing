#pragma once
#include "material.h"

namespace helper {
    template <typename T>
    struct YamlFetcher;

    template <typename T>
    inline T fetch(const Yaml& v, const T& d) {
        return YamlFetcher<T>::call(v, d);
    }

    template <typename T>
    inline T fetch(const Yaml& v) {
        return YamlFetcher<T>::call(v);
    }

    template <typename T>
    struct YamlFetcher {
        static T call(const Yaml& v, const T& d = T()) {
            if (!v || !v.IsScalar()) {
                return d;
            }
            return v.as<T>();
        }
    };

    template <>
    struct YamlFetcher<Vec> {
        static Vec call(const Yaml& v, const Vec& d = Vec(0)) {
            if (!v) {
                return d;
            }
            if (v.IsScalar()) {
                return Vec(v.as<double>());
            } else if (v.IsSequence()) {
                return Vec(v[0].as<double>(), v[1].as<double>(), v[2].as<double>());
            }
            return d;
        }
    };

    template <>
    struct YamlFetcher<Material> {
        static Material call(const Yaml& v, const Material& d = Material::diffuse()) {
            if (!v) {
                return d;
            }
            int k = v.IsScalar() | v.IsSequence() << 1;
            if (!k) {
                return d;
            }
            auto t = (k & 1 ? v : v[0]).as<std::string>();
            if (t == "diffuse") {
                return Material::diffuse();
            } else if (t == "specular") {
                return Material::specular();
            } else if (t == "refraction") {
                constexpr double n = 1.5;
                return Material::refraction(k & 1 ? n : fetch(v[1], n));
            }
            return d;
        }
    };

    template <typename T>
    auto bind(const Yaml& v) {
        return [&v] (auto&& i) {
            return fetch<T>(v[i]);
        };
    }
}
