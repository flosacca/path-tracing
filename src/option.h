#pragma once
#include "env.h"

struct Option {
    int spp = 1000;
    int ssaa = 2;
    int rounds = 1;
    int bounces = 4;
    double rr = 0.9;
    int rr_bounces = 20;

    Option() = default;

    Option(const Env& e) {
        auto f = env::bind(e);
        spp = f("spp", spp);
        ssaa = f("ssaa", ssaa);
        rounds = f("rounds", rounds);
        bounces = f("bounces", bounces);
        rr = f("rr", rr);
        rr_bounces = f("rr_bounces", rr_bounces);
    }
};
