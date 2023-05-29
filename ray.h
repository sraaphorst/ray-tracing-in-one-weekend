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

public:
    ray() noexcept = default;
    ray(const point3 &origin, const vec3 &direction) noexcept: orig{origin}, dir{direction} {}

    [[nodiscard]] point3 origin() const noexcept { return orig; }
    [[nodiscard]] vec3 direction() const noexcept { return dir; }

    [[nodiscard]] point3 at(const double t) const noexcept {
        return orig + t * dir;
    }
};
