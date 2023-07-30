/**
 * sphere.h
 * By Sebastian Raaphorst, 2023.
 */
#pragma once

#include "hittable.h"
#include "vec3.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <utility>

class sphere final : public hittable {
private:
    static void get_sphere_uv(const point3 &p, double &u, double &v) {
        const auto theta = std::acos(-p.y());
        const auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }

public:
    const point3 center;
    const double radius;
    shared_ptr<material> mat_ptr;

    sphere() noexcept: center{point3(0, 0, 0)}, radius{1} {}

    // Allow negative radius for S10.5.
    sphere(point3 center,
           double radius,
           shared_ptr<material> m) noexcept: center{center}, radius{radius}, mat_ptr{std::move(m)} {}

    [[nodiscard]] bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const noexcept override {
        const auto oc = r.origin() - center;
        const auto a = r.direction().length_squared();
        const auto half_b = oc.dot(r.direction());
        const auto c = oc.length_squared() - radius * radius;

        const auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        const auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root) {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        const auto outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat_ptr = mat_ptr;

        return true;
    }

    [[nodiscard]] bool bounding_box(double time0, double time1, aabb &output_box) const noexcept override {
        const auto v = vec3{radius, radius, radius};
        output_box = aabb{
            center - v,
            center + v
        };
        return true;
    }
};
