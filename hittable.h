/**
 * hittable.h
 * By Sebastian Raaphorst, 2023.
 */
#pragma once

#include <utility>

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

class translate final : public hittable {
public:
    shared_ptr<hittable> ptr;
    vec3 offset;

    translate(shared_ptr<hittable> ptr, const vec3 &offset) noexcept
    : ptr{std::move(ptr)}, offset{offset} {}

    [[nodiscard]] bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const noexcept override {
        const ray moved_r{r.origin() - offset, r.direction(), r.time()};
        if (!ptr->hit(moved_r, t_min, t_max, rec))
            return false;

        rec.p += offset;
        rec.set_face_normal(moved_r, rec.normal);
        return true;
    }

    [[nodiscard]] bool bounding_box(double time0, double time1, aabb &output_box) const noexcept override {
        if (!ptr->bounding_box(time0, time1, output_box))
            return false;

        output_box = aabb{output_box.minimum + offset, output_box.maximum + offset};
        return true;
    }
};


class rotate_y final : public hittable {
public:
    shared_ptr<hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    aabb bbox;

    rotate_y(const shared_ptr<hittable>& ptr, double angle): ptr{ptr} {
        const auto radians = degrees_to_radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        hasbox = ptr->bounding_box(0, 1, bbox);

        point3 min{ infinity,  infinity,  infinity};
        point3 max{-infinity, -infinity, -infinity};

        for (auto i=0; i < 2; ++i) {
            const auto x = i * bbox.maximum.x() + (1 - i) * bbox.minimum.x();
            for (auto j = 0; j < 2; ++j) {
                const auto y = j * bbox.maximum.y() + (1 - j) * bbox.minimum.y();
                for (auto k = 0; k < 2; ++k) {
                    const auto z = k * bbox.maximum.z() + (1 - k) * bbox.minimum.z();

                    const auto newx =  cos_theta * x + sin_theta * z;
                    const auto newz = -sin_theta * x + cos_theta * z;

                    const vec3 tester{newx, y, newz};
                    for (auto c = 0; c < 3; ++c) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb{min, max};
    }

    [[nodiscard]] bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const noexcept override {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

        const ray rotated_r{origin, direction, r.time()};
        if (!ptr->hit(rotated_r, t_min, t_max, rec))
            return false;

        auto p = rec.p;
        auto normal = rec.normal;

        p[0] =  cos_theta * rec.p[0] + sin_theta * rec.p[2];
        p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

        normal[0] =  cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
        normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

        rec.p = p;
        rec.set_face_normal(rotated_r, normal);
        return true;
    }

    [[nodiscard]] bool bounding_box(double time0, double tim1, aabb &output_box) const noexcept override {
        output_box = bbox;
        return hasbox;
    }
};
