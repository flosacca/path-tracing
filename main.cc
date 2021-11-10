#include "glm.h"
#include <cstdio>
#include <limits>
#include <vector>
#include <optional>

using Vec = glm::dvec3;

constexpr double INF = std::numeric_limits<double>::infinity();

inline void print(const Vec& a) {
    printf("(%.3f, %.3f, %.3f)\n", a.x, a.y, a.z);
}

struct Ray {
    Vec o, d;

    Ray(const Vec& o, const Vec& d) :
        o(o), d(d)
    {}

    Vec operator()(double t) const {
        return o + t * d;
    }
};

struct LocalData {
    Vec n;
};

enum Material {
    DIFFUSE,
    SPECULAR,
    REFRACTION,
};

// // BVH {{{
// struct Box {
//     Vec a, b;
//
//     bool intersects(const Ray& r) const {
//         float t1 = glm::compMax((a - r.o) / r.d);
//         float t2 = glm::compMin((b - r.o) / r.d);
//         return t1 < t2 && t2 > 0;
//     }
// };
//
// inline Box operator+(const Box& a, const Box& b) {
//     return {
//         glm::min(a.a, b.a),
//         glm::max(a.b, b.b)
//     };
// }
//
// template <typename T>
// struct BVH {
//     std::vector<T> objs;
//
//     template <typename It>
//     BVH(It first, It last) :
//         objs(first, last)
//     {}
//
//     template <typename F>
//     void each(const Ray& r, F f) const {
//         for (const T& e : objs) {
//             f(e);
//         }
//     }
// };
// // }}}
//
// // Mesh {{{
// struct Triangle {
//     Vec a, b, c;
//     Vec n;
//
//     Triangle(const Vec& a, const Vec& b, const Vec& c) :
//         a(a), b(b), c(c),
//         n(glm::normalize(glm::cross(b - a, c - a)))
//     {}
//
//     double intersection(const Ray& r) const {
//         Vec e1 = b - a;
//         Vec e2 = c - a;
//         Vec s = r.o - a;
//         Vec s1 = glm::cross(r.d, e2);
//         Vec s2 = glm::cross(s, e1);
//         Vec w {
//             glm::dot(s2, e2),
//             glm::dot(s1, s),
//             glm::dot(s2, r.d)
//         };
//         w *= 1.0 / glm::dot(s1, e1);
//         if (w.x > 0 && w.y > 0 && w.z > 0 && w.y + w.z < 1) {
//             return w.x;
//         }
//         return INF;
//     }
//
//     LocalData local() const {
//         return {n};
//     }
//
//     Box box() const {
//         return {
//             glm::min(glm::min(a, b), c),
//             glm::max(glm::max(a, b), c)
//         };
//     }
// };
//
// struct Mesh {
//     BVH<Triangle> bvh;
//     Material m;
//
//     template <typename It>
//     Mesh(It first, It last):
//         bvh(first, last)
//     {}
//
//     std::optional<std::pair<double, LocalData>> find(const Ray& r) const {
//         std::pair<double, const Triangle*> s { INF, nullptr };
//         bvh.each(r, [&] (const Triangle& e) {
//             double t = e.intersection(r);
//             s = std::min(s, {t, &e});
//         });
//         if (const Triangle* p = s.second) {
//             return {{s.first, p->local()}};
//         }
//         return {};
//     }
// };
// // }}}

// Sphere {{{
struct Sphere {
    double rad;
    Vec p, e, c;
    Material m;

    Sphere(double rad, Vec p, Vec e, Vec c, Material m):
        rad(rad), p(p), e(e), c(c), m(m)
    {}

    std::optional<std::pair<double, LocalData>> find(const Ray& r) const {
        Vec op = p - r.o;
        double t;
        constexpr double eps = 1e-4;
        double b = glm::dot(op, r.d);
        double det = b * b - glm::dot(op, op) + rad * rad;
        bool i = true;
        if (det < 0)
            i = false;
        else {
            det = sqrt(det);
            t = b - det;
            if (t <= eps) {
                t = b + det;
                if (t <= eps) {
                    i = false;
                }
            }
        }
        if (i) {
            LocalData local {glm::normalize(r(t) - p)};
            return {{t, local}};
        }
        return {};
    }
};

#define DIFF DIFFUSE
#define SPEC SPECULAR
#define REFR REFRACTION

Sphere spheres[] = {//Scene: radius, position, emission, color, material
  Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left
  Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght
  Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back
  Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//Frnt
  Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm
  Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top
  Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr
  Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas
  Sphere(600, Vec(50,681.6-.27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite
};
// }}}

#define rand() erand48(Xi)

Vec radiance(const Ray& r, int depth, uint16_t* Xi) {
    double t = INF;
    Vec n;
    const Sphere* obj = nullptr;
    for (const Sphere& e : spheres) {
        std::optional<std::pair<double, LocalData>> p = e.find(r);
        if (p && p->first < t) {
            t = p->first;
            n = p->second.n;
            obj = &e;
        }
    }
    if (!obj) {
        return {};
    }
    Vec x = r(t);
    bool into = glm::dot(n, r.d) < 0;
    Vec nl = into ? n : -n;
    Vec f = obj->c;
    double p = glm::compMax(f);
    ++depth;
    if (depth > 5) {
        if (rand() > p) {
            return obj->e;
        }
        f /= p;
    }
    if (obj->m == DIFFUSE) {
        double r1 = 2 * glm::pi<double>() * rand();
        double r2 = rand();
        double r2s = sqrt(r2);
        Vec w = nl;
        Vec u = glm::normalize(glm::cross(fabs(w.x) > 0.1 ? Vec(0, 1, 0) : Vec(1, 0, 0), w));
        Vec v = glm::cross(w, u);
        Vec d = glm::normalize(u * glm::cos(r1) * r2s + v * glm::sin(r1) * r2s + w * glm::sqrt(1 - r2));
        return obj->e + f * radiance(Ray(x, d), depth, Xi);
    }
    Ray reflRay(x, glm::reflect(r.d, n));
    if (obj->m == SPECULAR) {
        return obj->e + f * radiance(reflRay, depth, Xi);
    }
    double nc = 1;
    double nt = 1.5;
    double eta = nc / nt;
    double nnt = into ? eta : 1 / eta;
    double ddn = glm::dot(r.d, nl);
    double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
    if (cos2t < 0) {
        return obj->e + f * radiance(reflRay, depth, Xi);
    }
    Vec tdir = glm::normalize(r.d * nnt - nl * (ddn * nnt + glm::sqrt(cos2t)));
    double a = nt - nc;
    double b = nt + nc;
    double R0 = a * a / (b * b);
    double c = 1 - (into ? -ddn : glm::dot(tdir, n));
    double Re = R0 + (1 - R0) * glm::pow(c, 5);
    double Tr = 1 - Re;
    double P = 0.25 + 0.5 * Re;
    if (depth <= 2) {
        return obj->e + f * (radiance(reflRay, depth, Xi) * Re + radiance(Ray(x, tdir), depth, Xi) * Tr);
    }
    return obj->e + f * (rand() < P ? radiance(reflRay, depth, Xi) * (Re / P) : radiance(Ray(x, tdir), depth, Xi) * (Tr / (1 - P)));
}

int main(int argc, char *argv[]) {
    int w, h, samps = argc >= 2 ? atoi(argv[1]) / 4 : 50;
    if (argc >= 4) {
        w = atoi(argv[2]);
        h = atoi(argv[3]);
    } else {
        w = 160, h = 120;
    }
    Ray cam(Vec(50, 52, 295.6), glm::normalize(Vec(0, -0.042612, -1)));
    Vec cx = Vec(w * 0.5135 / h, 0, 0);
    Vec cy = glm::normalize(glm::cross(cx, cam.d)) * 0.5135;
    Vec r;
    Vec* c = new Vec[w * h];
    int k = 0;
#pragma omp parallel for schedule(dynamic, 1) private(r) num_threads(4)
    for (int y = 0; y < h; y++) {
        uint16_t Xi[3] = {0, 0, (uint16_t)((unsigned)y * y * y)};
        for (int x = 0; x < w; x++) {
            int i = (h - y - 1) * w + x;
            for (int sy = 0; sy < 2; sy++) {
                for (int sx = 0; sx < 2; sx++) {
                    r = Vec();
                    for (int s = 0; s < samps; s++) {
                        double r1 = 2 * rand(), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double r2 = 2 * rand(), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                        Vec d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
                                cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.d;
                        d = glm::normalize(d);
                        r += radiance(Ray(cam.o + d * 140.0, d), 0, Xi) * (1. / samps);
                    }
                    c[i] += glm::clamp(r, 0.0, 1.0) / 4.0;
                }
            }
            c[i] = glm::pow(c[i], Vec(1 / 2.2));
            k = std::max(k, y * w + x);
            fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100. * k / (h * w - 1));
        }
    }
    FILE *f = fopen("image.ppm", "w");
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
    for (int i = 0; i < w * h; i++) {
        Vec s = c[i] * 255.0 + 0.5;
        fprintf(f, "%d %d %d ", (int)s.x, (int)s.y, (int)s.z);
    }
}
