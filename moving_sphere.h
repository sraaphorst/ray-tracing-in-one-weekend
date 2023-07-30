/**
 * moving_sphere.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include "rtweekend.h"
#include "hittable.h"

class moving_sphere : public hittable {
public:
    point3 center0, center1;
    double time0, time1;
    double radius;
    shared_ptr<material> mat_ptr;

    moving_sphere() = default;
    moving_sphere(point3 center0,
                  point3 center1,
                  double time0,
                  double time1,
                  double radius,
                  shared_ptr<material> mat_ptr)
                  : center0{center0}, center1{center1}, time0{time0}, time1{time1}, radius{radius}, mat_ptr{mat_ptr} {}

    [[nodiscard]] auto center(double time) const {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

    [[nodiscard]] bool hit(const ray &r,
                           double t_min,
                           double t_max,
                           hit_record &rec) const noexcept override {
        const auto oc = r.origin() - center(r.time());
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
        const auto outward_normal = (rec.p - center(r.time())) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }

    [[nodiscard]] bool bounding_box(double _time0, double _time1, aabb &output_box) const noexcept override {
        const auto v = vec3{radius, radius, radius};
        aabb box0{
            center(_time0) - v,
            center(_time0) + v
        };
        aabb box1{
                center(_time1) - v,
                center(_time1) + v
        };
        output_box = surrounding_box(box0, box1);
        return true;
    }
};
