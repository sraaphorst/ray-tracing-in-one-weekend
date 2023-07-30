/**
 * hittable.h
 * By Sebastian Raaphorst, 2023.
 */
#pragma once

#include "aabb.h"
#include "ray.h"

class material;

struct hit_record final {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;

    // Coordinates for texture.
    double u;
    double v;

    bool front_face;

    inline void set_face_normal(const ray &r, const vec3 &outward_normal) noexcept {
        front_face = r.direction().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    [[nodiscard]] virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const noexcept = 0;
    [[nodiscard]] virtual bool bounding_box(double time0, double time1, aabb &output_box) const noexcept = 0;
};
