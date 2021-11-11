#include "glm.h"
#include "stb_image_write.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <optional>
// #include <vector>

using Vec = glm::dvec3;

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double PI = glm::pi<double>();

constexpr double EPS = 1e-4;

inline void print(const Vec& a) {
    printf("(%.3f, %.3f, %.3f)\n", a.x, a.y, a.z);
}

struct Ray {
    Vec o, d;

    Ray(const Vec& o, const Vec& d) :
        o(o), d(d) {}

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

struct Model {
    Vec e, c;
    Material m;

    Model(const Vec& e, const Vec& c, Material m) :
        e(e), c(c), m(m) {}

    virtual std::optional<std::pair<double, LocalData>> find(const Ray& r) const = 0;
};

struct Sphere : Model {
    double rad;
    Vec p;

    Sphere(double rad, const Vec& p, const Vec& c,
            Material m = DIFFUSE, const Vec& e = Vec(0)) :
        Model(e, c, m), rad(rad), p(p) {}

    std::optional<std::pair<double, LocalData>> find(const Ray& r) const {
        Vec op = p - r.o;
        double t;
        double b = glm::dot(op, r.d);
        double det = b * b - glm::dot(op, op) + rad * rad;
        bool i = true;
        if (det < 0)
            i = false;
        else {
            det = sqrt(det);
            t = b - det;
            if (t <= EPS) {
                t = b + det;
                if (t <= EPS) {
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

struct Plane : Model {
    Vec p, n;

    Plane(const Vec& p, const Vec& n, const Vec& c, Material m = DIFFUSE,
            const Vec& e = Vec(0))
        : Model(e, c, m), p(p), n(glm::normalize(n)) {}

    LocalData local() const { return {n}; }

    std::optional <std::pair<double, LocalData>> find(const Ray& r) const {
        double w = glm::dot(r.d, n);
        if (glm::abs(w) > EPS) {
            double t = glm::dot(p - r.o, n) / w;
            if (t > EPS) {
                return {{t, local()}};
            }
        }
        return {};
    }
};

Model* models[] = {
    new Plane(Vec(-50, 0, 0), Vec(-1, 0, 0), Vec(0.75, 0.25, 0.25)),
    new Plane(Vec(+50, 0, 0), Vec(+1, 0, 0), Vec(0.25, 0.25, 0.75)),
    new Plane(Vec(0, 0, 0), Vec(0, 0, +1), Vec(0.75)),
    new Plane(Vec(0, 0, 225), Vec(0, 0, -1), Vec(0.25, 0.75, 0.25)),
    new Plane(Vec(0, 0, 0), Vec(0, +1, 0), Vec(0.75)),
    new Plane(Vec(0, 100, 0), Vec(0, -1, 0), Vec(0.75)),
    new Sphere(16, Vec(-25, 16, 25), Vec(1), SPECULAR),
    new Sphere(16, Vec(+25, 16, 50), Vec(1), REFRACTION),
    new Sphere(600, Vec(0, 700 - 0.3, 50), Vec(0), DIFFUSE, Vec(12)),
};

struct Sampler {
    uint16_t* buf;

    Sampler(uint16_t* buf) : buf(buf) {}

    double uniform() {
        return erand48(buf);
    }

    double triangle() {
        double x = 2 * uniform() - 1;
        return (1 - glm::sqrt(1 - glm::abs(x))) * glm::sign(x);
    }

    Vec radiance(const Ray& r, int depth) {
        double t = INF;
        Vec n;
        Model* obj = nullptr;
        for (Model* m : models) {
            std::optional<std::pair<double, LocalData>> p = m->find(r);
            if (p && p->first < t) {
                t = p->first;
                n = p->second.n;
                obj = m;
            }
        }
        if (!obj) {
            return Vec(0);
        }
        Vec x = r(t);
        bool into = glm::dot(n, r.d) < 0;
        Vec nl = into ? n : -n;
        Vec f = obj->c;
        double p = glm::compMax(f);
        ++depth;
        if (depth > 5) {
            if (this->uniform() > p) {
                return obj->e;
            }
            f /= p;
        }
        if (obj->m == DIFFUSE) {
            double r1 = 2 * PI * this->uniform();
            double r2 = this->uniform();
            double r2s = sqrt(r2);
            Vec w = nl;
            double theta = glm::acos(w.z / glm::length(w));
            double phi = glm::atan(w.y, w.x);
            theta = fmod(theta + PI / 2, PI);
            Vec u = Vec {
                glm::cos(phi) * glm::sin(theta),
                glm::sin(phi) * glm::sin(theta),
                glm::cos(theta)
            };
            Vec v = glm::cross(w, u);
            Vec d = glm::normalize(u * glm::cos(r1) * r2s + v * glm::sin(r1) * r2s + w * glm::sqrt(1 - r2));
            return obj->e + f * radiance(Ray(x, d), depth);
        }
        Ray reflRay(x, glm::reflect(r.d, n));
        if (obj->m == SPECULAR) {
            return obj->e + f * radiance(reflRay, depth);
        }
        double nc = 1;
        double nt = 1.5;
        double eta = nc / nt;
        double nnt = into ? eta : 1 / eta;
        double ddn = glm::dot(r.d, nl);
        double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
        if (cos2t < 0) {
            return obj->e + f * radiance(reflRay, depth);
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
            return obj->e + f * (radiance(reflRay, depth) * Re + radiance(Ray(x, tdir), depth) * Tr);
        }
        return obj->e + f * (this->uniform() < P ? radiance(reflRay, depth) * (Re / P) : radiance(Ray(x, tdir), depth) * (Tr / (1 - P)));
    }
};

struct Camera {
    // Vec o, d, r, u;
    //
    // Vec(const Vec& o, const Vec& d, const Vec& u) :
    //     o(o),
    //     d(glm::normalize(d)),
    //     r(glm::normalize(glm::cross(d, u))),
    //     u(glm::cross(r, d))
    // {}

    Vec o, d, cx, cy;
};

struct Image {
    struct Pixel {
        int x, y;
        Sampler s;
        Vec c;

        Pixel(int x, int y, uint16_t* buf) :
            x(x), y(y), s(buf), c(0) {}

        void sample(int n, Image& im, const Camera& cam) {
            for (int sy = 0; sy < 2; ++sy) {
                for (int sx = 0; sx < 2; ++sx) {
                    Vec r(0);
                    for (int i = 0; i < n; ++i) {
                        double dx = s.triangle();
                        double dy = s.triangle();
                        double lx = ((sx + 0.5 + dx) / 2 + x) / im.w - 0.5;
                        double ly = ((sy + 0.5 + dy) / 2 + y) / im.h - 0.5;
                        Vec d = glm::normalize(cam.cx * lx + cam.cy * ly + cam.d);
                        r += s.radiance(Ray(cam.o, d), 0) * (1.0 / n);
                    }
                    c += glm::clamp(r, 0.0, 1.0) / 4.0;
                }
            }
        }

        uint32_t rgb() const {
            Vec r = glm::pow(c, Vec(1 / 2.2)) * 255.0 + 0.5;
            uint8_t t[4] = {(uint8_t) r.x, (uint8_t) r.y, (uint8_t) r.z};
            return *(uint32_t*) &t;
        }
    };

    int w, h;
    Pixel* p;

    Image(int w, int h) : w(w), h(h) {
        p = (Pixel*) malloc(w * h * sizeof(Pixel));
    }

    ~Image() {
        free(p);
    }

    void render(int spp, const Camera& cam) {
        int s = 0;
        #pragma omp parallel for schedule(dynamic, 1)
        for (int y = 0; y < h; y++) {
            uint16_t buf[3] = {0, 0, (uint16_t)((unsigned) y * y * y)};
            for (int x = 0; x < w; x++) {
                int i = (h - y - 1) * w + x;
                new (p + i) Pixel(x, y, buf);
                p[i].sample(spp / 4, *this, cam);
                #pragma omp atomic
                ++s;
                fprintf(stderr, "\rRendering (%d spp) %5.2f%%", spp, 100.0 * s / (w * h));
            }
        }
    }
};

int main(int argc, char** argv) {
    int w, h, spp = argc >= 2 ? atoi(argv[1]) : 100;
    if (argc >= 4) {
        w = atoi(argv[2]);
        h = atoi(argv[3]);
    } else {
        // w = 640, h = 480;
        // w = 320, h = 240;
        w = 160, h = 120;
        // w = 80, h = 60;
    }
    Vec o(0, 50, 225);
    Vec d = glm::normalize(Vec(0, 0, -1));
    double fov = glm::radians(45.0);
    double f = glm::tan(fov / 2) * 2;
    Vec cx = Vec((double) w / h, 0, 0) * f;
    Vec cy = glm::normalize(glm::cross(cx, d)) * f;
    Camera cam {o, d, cx, cy};
    Image im(w, h);
    im.render(spp, cam);
    uint8_t* buf = (uint8_t*) malloc(w * h * 3);
    for (int i = 0; i < w * h; i++) {
        uint32_t c = im.p[i].rgb();
        memcpy(buf + i * 3, &c, 3);
    }
    stbi_write_png("result.png", w, h, 3, buf, 0);
}
