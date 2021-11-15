#pragma once
#include "common.h"

class Sampler {
private:
    std::mt19937 g;

public:
    Sampler(uint32_t s) : g(s) {}

    double uniform(double l = 1.0) {
        return std::uniform_real_distribution<double>(0.0, l)(g);
    }

    double triangle() {
        double x = 2 * uniform() - 1;
        return (1 - glm::sqrt(1 - glm::abs(x))) * glm::sign(x);
    }
};
