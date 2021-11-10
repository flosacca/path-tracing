// #include "glm/glm.hpp"
#include "glm.h"
#include "glm/gtx/component_wise.hpp"
#include "glm/gtc/constants.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #include <cmath>
#include <cstdio>
#include <limits>
#include <type_traits>
#include <vector>
#include <optional>

using std::min;
using std::max;

using Vec = glm::dvec3;

// constexpr double INF = std::numeric_limits<double>::infinity();

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

// struct Sphere {
//     double rad;
//     Vec p, e, c;
//     Material m;
//
//     Sphere(double rad, Vec p, Vec e, Vec c, Material m):
//         rad(rad), p(p), e(e), c(c), m(m)
//     {}
//
//     std::optional<std::pair<double, LocalData>> find(const Ray& r) const {
//         Vec op = p - r.o;
//         double t;
//         constexpr double eps = 1e-4;
//         double b = glm::dot(op, r.d);
//         double det = b * b - glm::dot(op, op) + rad * rad;
//         bool i = true;
//         if (det < 0)
//             i = false;
//         else {
//             det = sqrt(det);
//             t = b - det;
//             if (t <= eps) {
//                 t = b + det;
//                 if (t <= eps) {
//                     i = false;
//                 }
//             }
//         }
//         if (i) {
//             return {{t, {glm::normalize(r(t) - p)}}};
//         }
//         return {};
//     }
// };

using Refl_t = Material;

struct Sphere {
    double rad;       // radius
    Vec p, e, c;      // position, emission, color
    Refl_t m;      // reflection type (DIFFuse, SPECular, REFRactive)
    Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_) :
        rad(rad_), p(p_), e(e_), c(c_), m(refl_) {}

    double intersect(const Ray& r) const { // returns distance, 0 if nohit
        Vec op = p - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        // double t, eps = 1e-4, b = op.dot(r.d), det = b * b - op.dot(op) + rad * rad;
        double t, eps = 1e-4, b = glm::dot(op, r.d), det = b * b - glm::dot(op, op) + rad * rad;
        if (det < 0)
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
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

inline bool intersect(const Ray& r, double& t, int& id) {
    double n = sizeof(spheres) / sizeof(Sphere), d, inf = t = 1e20;
    for (int i = int(n); i--;)
        if ((d = spheres[i].intersect(r)) && d < t) {
            t = d;
            id = i;
        }
    return t < inf;
}

// std::vector<Sphere> scene(spheres, spheres + sizeof spheres / sizeof(Sphere));

#define rand() erand48(Xi)

// Vec radiance(const Ray& r, int depth, uint16_t* Xi) {
//     // double t = INF;
//     // Vec n;
//     // const Sphere* obj = nullptr;
//     // for (const Sphere& e : scene) {
//     //     std::optional<std::pair<double, LocalData>> p = e.find(r);
//     //     if (p && p->first < t) {
//     //         t = p->first;
//     //         n = p->second.n;
//     //         obj = &e;
//     //     }
//     // }
//     // if (!obj) {
//     //     return {};
//     // }
//     double t;                               // distance to intersection
//     int id = 0;                             // id of intersected object
//     if (!intersect(r, t, id))
//         return Vec(); // if miss, return black
//     const Sphere* obj = &spheres[id];        // the hit object
//     Vec x = r(t);
//     Vec n = glm::normalize(x - obj->p);
//     bool into = glm::dot(n, r.d) < 0;
//     Vec nl = into ? n : -n;
//     Vec f = obj->c;
//     double p = glm::compMax(f);
//     ++depth;
//     if (depth > 5) {
//         if (rand() > p) {
//             return obj->e;
//         }
//         f /= p;
//     }
//     if (obj->m == DIFFUSE) {
//         double r1 = 2 * glm::pi<double>() * rand();
//         double r2 = rand();
//         double r2s = sqrt(r2);
//         Vec w = nl;
//         Vec u = glm::normalize(glm::cross(fabs(w.x) > 0.1 ? Vec(0, 1, 0) : Vec(1, 0, 0), w));
//         Vec v = glm::cross(w, u);
//         Vec d = glm::normalize(u * glm::cos(r1) * r2s + v * glm::sin(r1) * r2s + w * glm::sqrt(1 - r2));
//         return obj->e + f * radiance(Ray(x, d), depth, Xi);
//     }
//     Ray reflRay(x, glm::reflect(r.d, n));
//     if (obj->m == SPECULAR) {
//         return obj->e + f * radiance(reflRay, depth, Xi);
//     }
//     double nc = 1;
//     double nt = 1.5;
//     double eta = nc / nt;
//     double nnt = into ? eta : 1 / eta;
//     double ddn = glm::dot(r.d, nl);
//     double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
//     if (cos2t < 0) {
//         return obj->e + f * radiance(reflRay, depth, Xi);
//     }
//     Vec tdir = glm::normalize(r.d * nnt - nl * (ddn * nnt + glm::sqrt(cos2t)));
//     double a = nt - nc;
//     double b = nt + nc;
//     double R0 = a * a / (b * b);
//     double c = 1 - (into ? -ddn : glm::dot(tdir, n));
//     double Re = R0 + (1 - R0) * glm::pow(c, 5);
//     double Tr = 1 - Re;
//     double P = 0.25 + 0.5 * Re;
//     if (depth <= 2) {
//         return obj->e + f * (radiance(reflRay, depth, Xi) * Re + radiance(Ray(x, tdir), depth, Xi) * Tr);
//     }
//     return obj->e + f * (rand() < P ? radiance(reflRay, depth, Xi) * (Re / P) : radiance(Ray(x, tdir), depth, Xi) * (Tr / (1 - P)));
// }

Vec operator%(Vec a, Vec b) {
    return glm::cross(a, b);
}

Vec norm(Vec a) {
    return glm::normalize(a);
}

#include <cmath>

Vec radiance(const Ray& r, int depth, unsigned short *Xi) {
    double t;                               // distance to intersection
    int id = 0;                             // id of intersected object
    if (!intersect(r, t, id))
        return Vec(); // if miss, return black
    const Sphere& obj = spheres[id];        // the hit object
    Vec x = r.o + r.d * t, n = norm(x - obj.p), nl = glm::dot(n, r.d) < 0 ? n : -n, f = obj.c;
    double p = std::max(f.x, std::max(f.y, f.z)); // max refl
    if (++depth > 5)
        if (erand48(Xi) < p)
            f = f * (1 / p);
        else
            return obj.e; //R.R.
    if (obj.m == DIFF) {                 // Ideal DIFFUSE reflection
        double r1 = 2 * M_PI * erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
        Vec w = nl, u = norm((fabs(w.x) > .1 ? Vec(0, 1, 0) : Vec(1, 0, 0)) % w), v = w % u;
        Vec d = norm(u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2));
        return obj.e + f * (radiance(Ray(x, d), depth, Xi));
    }
    if (obj.m == SPEC)                   // Ideal SPECULAR reflection
        return obj.e + f * (radiance(Ray(x, r.d - n * 2.0 * glm::dot(n, r.d)), depth, Xi));
    Ray reflRay(x, r.d - n * 2.0 * glm::dot(n, r.d)); // Ideal dielectric REFRACTION
    bool into = glm::dot(n, nl) > 0;              // Ray from outside going in?
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = glm::dot(r.d, nl), cos2t;
    if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) // Total internal reflection
        return obj.e + f * (radiance(reflRay, depth, Xi));
    Vec tdir = norm(r.d * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t))));
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : glm::dot(tdir, n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
    return obj.e + f * (depth > 2 ? (erand48(Xi) < P ? // Russian roulette
                                       radiance(reflRay, depth, Xi) * RP : radiance(Ray(x, tdir), depth, Xi) * TP) :
                          radiance(reflRay, depth, Xi) * Re + radiance(Ray(x, tdir), depth, Xi) * Tr);
}

inline double clamp(double x) {
    return x < 0 ? 0 : x > 1 ? 1 : x;
}

inline int toInt(double x) {
    return int(pow(clamp(x), 1 / 2.2) * 255 + .5);
}

int main(int argc, char *argv[]) {
    int w, h, samps = argc >= 2 ? atoi(argv[1]) / 4 : 50; // # samples
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
#pragma omp parallel for schedule(dynamic, 1) private(r) num_threads(4)       // OpenMP
    for (int y = 0; y < h; y++) {                  // Loop over image rows
        unsigned short Xi[3] = {0, 0, (unsigned short)((unsigned)y * y * y)};
        for (int x = 0; x < w; x++) { // Loop cols
            for (int sy = 0, i = (h - y - 1) * w + x; sy < 2; sy++) // 2x2 subpixel rows
                for (int sx = 0; sx < 2; sx++, r = Vec()) { // 2x2 subpixel cols
                    for (int s = 0; s < samps; s++) {
                        double r1 = 2 * erand48(Xi), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double r2 = 2 * erand48(Xi), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                        Vec d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
                                cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.d;
                        d = glm::normalize(d);
                        r = r + radiance(Ray(cam.o + d * 140.0, d), 0, Xi) * (1. / samps);
                    } // Camera rays are pushed ^^^^^ forward to start in interior
                    c[i] = c[i] + Vec(clamp(r.x), clamp(r.y), clamp(r.z)) * .25;
                }
            k = std::max(k, y * w + x);
            fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100. * k / (h * w - 1));
        }
    }
    FILE *f = fopen("image.ppm", "w");         // Write image to PPM file.
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
    for (int i = 0; i < w * h; i++)
        fprintf(f, "%d %d %d ", toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));
}
