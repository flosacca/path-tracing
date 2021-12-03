#pragma once
#include "common.h"

class Sampler {
private:
    std::mt19937 g;

public:
    Sampler(uint32_t s) : g(s) {}

    float uniform(float l = 1.0f) {
        return std::uniform_real_distribution<float>(0.0f, l)(g);
    }

    float triangle() {
        float x = 2 * uniform() - 1;
        return (1 - glm::sqrt(1 - glm::abs(x))) * glm::sign(x);
    }
};
