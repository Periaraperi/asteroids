#pragma once

#include <random>

namespace peria {
    inline std::random_device rd = std::random_device();
    inline std::mt19937 generator(rd());

    [[nodiscard]]
    static inline
    int get_int(int l, int r)
    {
        std::uniform_int_distribution<> dist(l, r);
        return dist(rd);
    }

    [[nodiscard]]
    static inline
    float get_float(float l, float r)
    {
        std::uniform_real_distribution<float> dist(l, r);
        return dist(rd);
    }
}
