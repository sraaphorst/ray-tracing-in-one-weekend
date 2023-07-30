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
    vec3 u, v, w;
    double lens_radius;
    double time0;
    double time1;

public:
    camera(point3 lookfrom,
           point3 lookat,
           vec3 vup,
           double vertical_fov,
           double aspect_ratio,
           double aperture,
           double focus_dist,
           double time0 = 0.0,
           double time1 = 0.0) : time0{time0}, time1{time1} {
        const auto theta = degrees_to_radians(vertical_fov);
        const auto h = std::tan(theta / 2);
        const auto viewport_height = 2.0 * h;
        const auto viewport_width = aspect_ratio * viewport_height;

        w = (lookfrom - lookat).unit_vector();
        u = vup.cross(w).unit_vector();
        v = w.cross(u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
    }

    [[nodiscard]] ray get_ray(double s, double t) const noexcept {
        const auto rd = lens_radius * random_in_unit_disk();
        const auto offset = u * rd.x() + v * rd.y();
        return {origin + offset,
                lower_left_corner + s * horizontal + t * vertical - origin - offset,
                random_double(time0, time1)};
    }
};

const double camera::focal_length = 1.0;