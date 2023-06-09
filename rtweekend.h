/**
 * rtweekend.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const auto infinity = std::numeric_limits<double>::infinity();
const auto pi = M_PI;

[[nodiscard]] inline auto degrees_to_radians(double degrees) noexcept {
    return degrees * pi / 180.0;
}

[[nodiscard]] inline double clamp(double x, double min, double max) {
    return std::min(std::max(x, min), max);
}

namespace global_rng {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distribution(0.0, 1.0);
}

[[nodiscard]] inline auto random_double() noexcept {
    return global_rng::distribution(global_rng::generator);
}

[[nodiscard]] inline auto random_double(double min, double max) {
    return min + (max - min) * random_double();
}
