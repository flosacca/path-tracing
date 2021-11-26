#pragma once
#include "env.h"

struct Option {
    int spp = 1000;
    int ssaa = 2;
    int bagging = 1;
    int bounce = 8;
    double rr = 0;
    int rr_bounce = 0;

    Option() = default;

    Option(const Env& e) {
        auto f = env::bind(e);
        spp = f("spp", spp);
        ssaa = f("ssaa", ssaa);
        bagging = f("bagging", bagging);
        bounce = f("bounce", bounce);
        rr = f("rr", rr);
        rr_bounce = f("rr_bounce", rr_bounce);
    }
};
