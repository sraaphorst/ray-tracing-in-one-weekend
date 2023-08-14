/**
 * constant_medium.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include <utility>

#include "rtweekend.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium final : public hittable {
public:
    shared_ptr<hittable> boundary;
    shared_ptr<material> phase_function;
    double neg_inv_density;

    constant_medium(shared_ptr<hittable> boundary,
                    double density,
                    const shared_ptr<texture>& texture) noexcept
                    : boundary{std::move(boundary)},
                      neg_inv_density{-1.0/density},
                      phase_function{make_shared<isotropic>(texture)} {}

    constant_medium(shared_ptr<hittable> boundary,
                    double density,
                    color c) noexcept
                    : boundary{std::move(boundary)},
                      neg_inv_density{-1.0/density},
                      phase_function{make_shared<isotropic>(c)} {}

    [[nodiscard]] bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const noexcept override {
        // Print occasional samples when debugging. Set enableDebug to true.
        constexpr bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 1e-5;

        hit_record rec1;
        if (!boundary->hit(r, -infinity, infinity, rec1))
            return false;

        hit_record rec2;
        if (!boundary->hit(r, rec1.t + 1e-4, infinity, rec2))
            return false;

        if (debugging)
            std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

        if (rec1.t < t_min) rec1.t = t_min;
        if (rec2.t > t_max) rec2.t = t_max;
        if (rec1.t > rec2.t)
            return false;

        if (rec1.t < 0) rec1.t = 0;

        const auto ray_length = r.direction().length();
        const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;

        const auto hit_distance = neg_inv_density * log(random_double());
        if (hit_distance > distance_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging)
            std::cerr << "hit_distance=" << hit_distance << '\n'
                      << "rec.t=" << rec.t << '\n'
                      << "rec.p=" << rec.p << '\n';

        // These values are arbitrary.
        rec.normal = vec3{1, 0, 0};
        rec.front_face = true;
        rec.mat_ptr = phase_function;

        return true;
    };

    [[nodiscard]] bool bounding_box(double time0, double time1, aabb &output_box) const noexcept override {
        return boundary->bounding_box(time0, time1, output_box);
    }
};
