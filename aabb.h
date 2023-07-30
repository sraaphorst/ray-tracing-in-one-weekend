/**
 * aabb.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include <cmath>

#include "ray.h"
#include "vec3.h"

class aabb {
public:
    point3 minimum;
    point3 maximum;

    aabb() noexcept = default;
    aabb(const point3 &a, const point3 &b) noexcept: minimum{a}, maximum{b} {}

    [[nodiscard]] inline bool hit(const ray &r, double t_min, double t_max) const noexcept {
        for (auto a = 0; a < 3; ++a) {
            const auto invD = 1.0f / r.direction()[a];
            auto t0 = (minimum[a] - r.origin()[a]) * invD;
            auto t1 = (maximum[a] - r.origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }
};

aabb surrounding_box(aabb box0, aabb box1) {
    point3 small{std::fmin(box0.minimum.x(), box1.minimum.x()),
                 std::fmin(box0.minimum.y(), box1.minimum.y()),
                 std::fmin(box0.minimum.z(), box1.minimum.z())};
    point3 big{std::fmax(box0.maximum.x(), box1.maximum.x()),
               std::fmax(box0.maximum.y(), box1.maximum.y()),
               std::fmax(box0.maximum.z(), box1.maximum.z())};
    return aabb{small, big};
}
