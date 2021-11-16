#include "mesh.h"
#include "camera.h"
#include "device.h"

int main(int argc, char** argv) {
    int w = 800;
    int h = w / 4 * 3;
    int spp = 1000;
    auto conf = YAML::LoadFile(argc > 1 ? argv[1] : "config.yml");
    if (auto meta = conf["image"]) {
        if (auto size = meta["size"]) {
            w = size[0].as<int>();
            h = size[1].as<int>();
        }
        spp = helper::fetch(meta["spp"], spp);
    }
    Scene scene = Scene::load(conf["scene"]);
    Camera cam(Vec(0, 50, 225), Vec(0, 0, -1), (double) w / h);
    Image im(w, h);
    cam.render(scene, im, spp);
    im.save("output.png");
}
