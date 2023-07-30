/**
 * hittable_list.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include "hittable.h"

#include <memory>
#include <utility>
#include <vector>

class hittable_list final : public hittable {
public:
    std::vector<std::shared_ptr<hittable>> objects;

    hittable_list() = default;
    explicit hittable_list(const std::shared_ptr<hittable>& object) {
        add(object);
    }

    void clear() {
        objects.clear();
    }

    void add(const std::shared_ptr<hittable>& object) {
        objects.emplace_back(object);
    }

    [[nodiscard]] bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const noexcept override {
        hit_record temp_rec;
        auto hit_anything = false;
        auto closest_so_far = t_max;

        for (const auto &object: objects) {
            if (object->hit(r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    [[nodiscard]] bool bounding_box(double time0, double time1, aabb &output_box) const noexcept override {
        if (objects.empty())
            return false;

        aabb temp_box;
        bool first_box = true;

        for (const auto &object: objects) {
            if (!object->bounding_box(time0, time1, temp_box)) return false;
            output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
            first_box = false;
        }

        return true;
    }
};