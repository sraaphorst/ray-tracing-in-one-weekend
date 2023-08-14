/**
 * ray.h
 * By Sebastian Raaphorst, 2023.
 */
#pragma once

#include "vec3.h"

class ray final {
private:
    point3 orig;
    vec3 dir;
    double tm;

public:
    ray() noexcept = default;
    ray(const point3 &origin,
        const vec3 &direction) noexcept: orig{origin}, dir{direction}, tm{0} {}
    ray(const point3 &origin,
        const vec3 &direction,
        double time = 0.0) noexcept: orig{origin}, dir{direction}, tm{time} {}

    [[nodiscard]] auto origin() const noexcept { return orig; }
    [[nodiscard]] auto direction() const noexcept { return dir; }
    [[nodiscard]] auto time() const noexcept { return tm; }

    [[nodiscard]] auto at(const double t) const noexcept {
        return orig + t * dir;
    }
};
