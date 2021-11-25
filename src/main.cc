#include <omp.h>
#include "camera.h"

int main(int argc, char** argv) {
    int w = 800;
    int h = w / 4 * 3;
    auto conf = YAML::LoadFile(argc > 1 ? argv[1] : "config.yml");
    if (int n = env::fetch(conf["threads"], 0)) {
        omp_set_num_threads(n);
    }
    auto bm = conf["bm"];
    std::string p = "output.png";
    Option opt;
    if (auto e = conf["rendering"]) {
        p = env::fetch(e["output"], p);
        if (auto size = e["size"]) {
            w = size[0].as<int>();
            h = size[1].as<int>();
        }
        opt = Option(e);
    }
    Scene scene = Scene::load(conf["scene"]);
    Camera cam = Camera::load(conf["camera"], (double) w / h);
    Image im(w, h);
    cam.render(scene, im, opt);
    im.save(p);
}
