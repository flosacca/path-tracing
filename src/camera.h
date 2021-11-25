#pragma once
#include "env.h"
#include "tracer.h"

class Camera {
private:
    Vec origin, dir, right, up;

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
    void render(const Scene& scene, Device& device, int spp) {
        // static FILE* log_file = fopen("camera.log", "w");
        int n = spp / 4;
        int w = device.width();
        int h = device.height();
        fprintf(stderr, "Start rendering with width = %d, height = %d, spp = %d\n", w, h, spp);
        int m = RayTracer::opts.m;
        int p = 0;
        #pragma omp parallel for schedule(dynamic, 1)
        for (int y = 0; y < h; y++) {
            Sampler samp((uint32_t) y * y);
            RayTracer tracer(scene, samp);
            for (int x = 0; x < w; x++) {
                Vec c(0);
                for (int k = 0; k < 4; ++k) {
                    int i = 0;
                    for (int j = 1; j <= m; ++j) {
                        Vec s(0);
                        double i0 = i;
                        // (i / n) < (j / m)
                        for (; i * m < j * n; ++i) {
                            double dx = samp.triangle();
                            double dy = samp.triangle();
                            double nx = (x * 2 + dx + k % 2 + 0.5) / w - 1;
                            double ny = (y * 2 + dy + k / 2 + 0.5) / h - 1;
                            Ray ray(origin, glm::normalize(nx * right + ny * up + dir));
                            // Vec rad = tracer.radiance(ray);
                            // if (glm::compMin(rad) > 1) {
                            //     fprintf(log_file, "%d %d %d %d %d %f %f %f\n", x, h - y - 1, i, j, k, rad.x, rad.y, rad.z);
                            // }
                            s += tracer.radiance(ray);
                        }
                        c += glm::clamp(s / (i - i0), 0.0, 1.0);
                    }
                }
                device.set(x, h - y - 1, c / (m * 4.0));
                #pragma omp atomic
                ++p;
                fprintf(stderr, "\rRendered: %5.2f%%", 100.0 * p / (w * h));
                if (p == w * h) {
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
