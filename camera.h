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
    static const double focal_length;

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

public:
    camera(point3 lookfrom,
           point3 lookat,
           vec3 vup,
           double vertical_fov,
           double aspect_ratio) {
        const auto theta = degrees_to_radians(vertical_fov);
        const auto h = std::tan(theta / 2);
        const auto viewport_height = 2.0 * h;
        const auto viewport_width = aspect_ratio * viewport_height;

        const auto w = (lookfrom - lookat).unit_vector();
        const auto u = vup.cross(w).unit_vector();
        const auto v = w.cross(u);

        origin = lookfrom;
        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
    }

    [[nodiscard]] ray get_ray(double s, double t) const noexcept {
        return {origin, lower_left_corner + s * horizontal + t * vertical - origin};
    }
};

const double camera::focal_length = 1.0;