/**
 * hittable_list.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list final : public hittable {
public:
    std::vector<std::shared_ptr<hittable>> objects;

    hittable_list() = default;
    hittable_list(std::shared_ptr<hittable> object) {
        add(object);
    }

    void clear() {
        objects.clear();
    }

    void add(std::shared_ptr<hittable> object) {
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
};