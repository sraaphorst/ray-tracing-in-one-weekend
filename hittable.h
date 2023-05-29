/**
 * hittable.h
 * By Sebastian Raaphorst, 2023.
 */
#pragma once

#include "ray.h"

struct hit_record final {
    point3 p;
    vec3 normal;
    double t;
    bool front_face;

    inline void set_face_normal(const ray &r, const vec3 &outward_normal) noexcept {
        front_face = r.direction().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    [[nodiscard]] virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const noexcept = 0;
};
