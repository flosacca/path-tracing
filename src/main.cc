#include <omp.h>
#include "mesh.h"
#include "camera.h"

int main(int argc, char** argv) {
    int w = 800;
    int h = w / 4 * 3;
    int spp = 1000;
    auto conf = YAML::LoadFile(argc > 1 ? argv[1] : "config.yml");
    if (int n = hlp::fetch(conf["threads"], 0)) {
        omp_set_num_threads(n);
    }
    std::string p = "output.png";
    if (auto meta = conf["image"]) {
        if (auto size = meta["size"]) {
            w = size[0].as<int>();
            h = size[1].as<int>();
        }
        spp = hlp::fetch(meta["spp"], spp);
        p = hlp::fetch(meta["output"], p);
    }
    Scene scene = Scene::load(conf["scene"]);
    Camera cam = Camera::load(conf["camera"], (double) w / h);
    Image im(w, h);
    cam.render(scene, im, spp);
    im.save(p);
}
