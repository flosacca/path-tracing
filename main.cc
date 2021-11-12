#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "stb_image_write.h"
#include "model.h"

Model* models[] = {
    new Plane(Vec(-1, 0, 0), Vec(-50, 0, 0), Vec(0.75, 0.25, 0.25)),
    new Plane(Vec(+1, 0, 0), Vec(+50, 0, 0), Vec(0.25, 0.25, 0.75)),
    new Plane(Vec(0, 0, +1), Vec(0, 0, 0), Vec(0.75)),
    new Plane(Vec(0, 0, -1), Vec(0, 0, 225), Vec(0.25, 0.75, 0.25)),
    new Plane(Vec(0, +1, 0), Vec(0, 0, 0), Vec(0.75)),
    new Plane(Vec(0, -1, 0), Vec(0, 100, 0), Vec(0.75)),
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
        Intersection i;
        for (Model* m : models) {
            i = std::min(i, m->find(r));
        }
        if (!i.m) {
            return Vec(0);
        }
        double t = i.t;
        Vec n = i.n;
        Vec x = r(t);
        bool into = glm::dot(n, r.d) < 0;
        Vec nl = into ? n : -n;
        Vec f = i.m->c;
        double p = glm::compMax(f);
        ++depth;
        if (depth > 5) {
            if (this->uniform() > p) {
                return i.m->e;
            }
            f /= p;
        }
        if (i.m->m == DIFFUSE) {
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
            return i.m->e + f * radiance(Ray(x, d), depth);
        }
        Ray reflRay(x, glm::reflect(r.d, n));
        if (i.m->m == SPECULAR) {
            return i.m->e + f * radiance(reflRay, depth);
        }
        double nc = 1;
        double nt = 1.5;
        double eta = nc / nt;
        double nnt = into ? eta : 1 / eta;
        double ddn = glm::dot(r.d, nl);
        double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
        if (cos2t < 0) {
            return i.m->e + f * radiance(reflRay, depth);
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
            return i.m->e + f * (radiance(reflRay, depth) * Re + radiance(Ray(x, tdir), depth) * Tr);
        }
        return i.m->e + f * (this->uniform() < P ? radiance(reflRay, depth) * (Re / P) : radiance(Ray(x, tdir), depth) * (Tr / (1 - P)));
    }
};

class Camera {
private:
    Vec o, d, i, j;

public:
    Camera(const Vec& origin,
        const Vec& direction,
        double fov,
        double aspect,
        const Vec& worldUp = Vec(0, 1, 0)) :
        o(origin),
        d(direction),
        i(glm::cross(d, worldUp)),
        j(glm::cross(i, d))
    {
        double w = glm::tan(fov / 2);
        i *= w / glm::length(i) * aspect;
        j *= w / glm::length(j);
    }

    Ray to(double x, double y) const {
        return {o, glm::normalize(x * i + y * j + d)};
    }
};

struct Image {
    struct Pixel {
        int x, y;
        Sampler samp;
        Vec c;

        Pixel(int x, int y, uint16_t* buf) :
            x(x), y(y), samp(buf), c(0) {}

        void sample(int n, Image& im, const Camera& cam) {
            for (int j = 0; j < 2; ++j) {
                for (int i = 0; i < 2; ++i) {
                    Vec r(0);
                    for (int k = 0; k < n; ++k) {
                        double dx = samp.triangle();
                        double dy = samp.triangle();
                        double nx = (x * 2 + dx + i + 0.5) / im.w - 1;
                        double ny = (y * 2 + dy + j + 0.5) / im.h - 1;
                        r += samp.radiance(cam.to(nx, ny), 0) * (1.0 / n);
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
    Camera cam(Vec(0, 50, 225), Vec(0, 0, -1), glm::radians(45.0), (double) w / h);
    Image im(w, h);
    im.render(spp, cam);
    uint8_t* buf = (uint8_t*) malloc(w * h * 3);
    for (int i = 0; i < w * h; i++) {
        uint32_t c = im.p[i].rgb();
        memcpy(buf + i * 3, &c, 3);
    }
    stbi_write_png("output.png", w, h, 3, buf, 0);
}
