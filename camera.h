/**
 * camera.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"

class camera final {
public:
    static const point3 origin;
    static const double aspect_ratio;
    static const double viewport_height;
    static const double focal_length;

private:
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

public:
    camera() {
        const auto viewport_width = aspect_ratio * viewport_height;
        horizontal = vec3(viewport_width, 0.0, 0.0);
        vertical = vec3(0.0, viewport_height, 0.0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);
    }

    [[nodiscard]] ray get_ray(double u, double v) const noexcept {
        return {origin, lower_left_corner + u * horizontal + v * vertical - origin};
    }
};

const point3 camera::origin{0, 0, 0};
const double camera::aspect_ratio = 16.0 / 9.0;
const double camera::viewport_height = 2.0;
const double camera::focal_length = 1.0;