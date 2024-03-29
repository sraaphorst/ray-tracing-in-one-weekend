/**
 * box.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include "rtweekend.h"
#include "aarect.h"
#include "hittable_list.h"

class box final : public hittable {
public:
    point3 box_min;
    point3 box_max;
    hittable_list sides;

    box() noexcept = default;
    box(const point3 &p0, const point3 &p1, shared_ptr<material> ptr) {
        box_min = p0;
        box_max = p1;

        sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
        sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

        sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
        sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

        sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
        sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
    }

    [[nodiscard]] bool bounding_box(double time0, double time1, aabb &output_box) const noexcept override {
        output_box = aabb(box_min, box_max);
        return true;
    }

    [[nodiscard]] bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const noexcept override {
        return sides.hit(r, t_min, t_max, rec);
    }
};
