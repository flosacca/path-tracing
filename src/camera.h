#pragma once
#include "tracer.h"

class Camera {
private:
    Vec origin, dir, right, up;

public:
    Camera(const Vec& origin,
        const Vec& direction,
        double aspect,
        double fov = PI / 4,
        const Vec& worldUp = Vec(0, 1, 0)) :
        origin(origin),
        dir(direction),
        right(glm::cross(dir, worldUp)),
        up(glm::cross(right, dir))
    {
        double k = glm::tan(fov / 2);
        right *= k / glm::length(right) * aspect;
        up *= k / glm::length(up);
    }

    template <typename Device>
    void render(const Scene& scene, Device& device, int spp) {
        int n = spp / 4;
        int w = device.width();
        int h = device.height();
        int s = 0;
        #pragma omp parallel for schedule(dynamic, 1)
        for (int y = 0; y < h; y++) {
            Sampler samp((uint32_t) y * y);
            RayTracer tracer(scene, samp);
            for (int x = 0; x < w; x++) {
                Vec c(0);
                for (int j = 0; j < 2; ++j) {
                    for (int i = 0; i < 2; ++i) {
                        Vec r(0);
                        for (int k = 0; k < n; ++k) {
                            double dx = samp.triangle();
                            double dy = samp.triangle();
                            double nx = (x * 2 + dx + i + 0.5) / w - 1;
                            double ny = (y * 2 + dy + j + 0.5) / h - 1;
                            Ray ray(origin, glm::normalize(nx * right + ny * up + dir));
                            r += tracer.radiance(ray, 0);
                        }
                        c += glm::clamp(r / (double) n, 0.0, 1.0);
                    }
                }
                device.set(x, y, c / 4.0);
                #pragma omp atomic
                ++s;
                fprintf(stderr, "\rRendering (%d spp) %5.2f%%", spp, 100.0 * s / (w * h));
                if (s == w * h) {
                    fputs("\n", stderr);
                }
            }
        }
    }
};
