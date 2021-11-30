#pragma once
#include "tracer.h"

class Camera {
private:
    Vec origin, dir, right, up;

    static double transform(double p, int n, int i, int j, double d) {
        // (i + j * p + p / 2 + d / 2) / n * 2 - 1
        return (i * 2 + (j * 2 + 1) * p + d) / n - 1;
    }

public:
    Camera(const Vec& origin,
           const Vec& direction,
           double aspect,
           double fov = PI / 4,
           const Vec& worldUp = Vec(0, 1, 0))
        : origin(origin),
          dir(direction),
          right(glm::cross(dir, worldUp)),
          up(glm::cross(right, dir)) {
        double k = glm::tan(fov / 2);
        right *= k / glm::length(right) * aspect;
        up *= k / glm::length(up);
    }

    template <typename Device>
    void render(const Scene& scene, Device& device, const Option& opt) {
        int s1 = opt.ssaa;
        int s2 = num::pow<2>(s1);
        double inv_s1 = 1.0 / s1;
        int n = opt.spp / s2;
        int w = device.width();
        int h = device.height();
        fprintf(stderr, "Start rendering with width = %d, height = %d, spp = %d\n", w, h, n * s2);
        int m = opt.rounds;
        int tot = 0;
        #pragma omp parallel for schedule(dynamic, 1)
        for (int y = h - 1; y >= 0; --y) {
            Sampler samp(num::pow<2>((uint32_t) y));
            RayTracer tracer(scene, samp, opt);
            for (int x = w - 1; x >= 0; --x) {
                Vec c(0);
                for (int k = s2 - 1; k >= 0; --k) {
                    int i = 0;
                    for (int j = 1; j <= m; ++j) {
                        Vec sum(0);
                        double i0 = i;
                        // (i / n) < (j / m)
                        for (; i * m < j * n; ++i) {
                            double nx = transform(inv_s1, w, x, k % s1, samp.triangle());
                            double ny = transform(inv_s1, h, y, k / s1, samp.triangle());
                            sum += tracer.radiance(Ray(origin, glm::normalize(nx * right + ny * up + dir)));
                        }
                        c += glm::clamp(sum / (i - i0), 0.0, 1.0);
                    }
                }
                device.set(x, h - y - 1, c * (1.0 / (m * s2)));
                #pragma omp atomic
                ++tot;
                fprintf(stderr, "\rRendered: %5.2f%%", 100.0 * tot / (w * h));
                if (tot == w * h) {
                    fputs("\n", stderr);
                }
            }
        }
    }

    static Camera load(const Env& t, double aspect) {
        auto v = env::accessor(Vec(0))(t);
        Vec o = v("origin");
        Vec d = v("direction");
        double fov = glm::radians(env::fetch(t["fov"], 45.0));
        return Camera(o, d, aspect, fov);
    }
};
