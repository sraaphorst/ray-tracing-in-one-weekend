/**
 * perlin.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include <array>
#include <vector>

#include "rtweekend.h"
#include "vec3.h"

class perlin final {
private:
    using noise_array = std::array<std::array<std::array<double, 2>, 2>, 2>;

    static const int point_count = 256;
    std::vector<float> randfloat;
    std::vector<int> perm_x;
    std::vector<int> perm_y;
    std::vector<int> perm_z;

    static void perlin_generate_perm(std::vector<int> &p) {
        std::iota(std::begin(p), std::end(p), 0);

        for (auto i = p.size()-1; i > 0; --i) {
            int target = random_int(0, 1);
            std::swap(p[i], p[target]);
        }
    }

    [[nodiscard]] static double trilinear_interp(noise_array &c, double u, double v, double w) {
        auto accum = 0.0;
        for (auto i = 0; i < 2; ++i)
            for (auto j = 0; j < 2; ++j)
                for (auto k = 0; k < 2; ++k)
                    accum += (i * u + (1 - i) * (1 - u))
                             * (j * v + (1 - j) * (1 - v))
                             * (k * w + (1 - k) * (1 - w))
                             * c[i][j][k];

        return accum;
    }

public:
    perlin() noexcept
    : randfloat(point_count), perm_x(point_count), perm_y(point_count), perm_z(point_count) {
        std::generate(std::begin(randfloat), std::end(randfloat), random_float);
        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    [[nodiscard]] auto noise(const point3 &p) const noexcept {
        const auto fpx = std::floor(p.x());
        const auto fpy = std::floor(p.y());
        const auto fpz = std::floor(p.z());

        auto u = p.x() - fpx;
        auto v = p.y() - fpy;
        auto w = p.z() - fpz;

        // Hermite cubic to round off interpolation.
        u = u * u * (3 - 2 * u);
        v = v * v * (3 - 2 * v);
        w = w * w * (3 - 2 * w);

        const auto i = static_cast<int>(fpx);
        const auto j = static_cast<int>(fpy);
        const auto k = static_cast<int>(fpz);

        noise_array c;
        for (auto di = 0; di < 2; ++di)
            for (auto dj = 0; dj < 2; ++dj)
                for (auto dk = 0; dk < 2; ++dk)
                    c[di][dj][dk] = randfloat[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];

        return trilinear_interp(c, u, v, w);
    }
};
