/**
 * material.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include "ray.h"
#include "hittable.h"

class material {
public:
    [[nodiscard]] virtual bool scatter(
            const ray &r_in,
            const hit_record &rec,
            color &attenuation,
            ray &scattered) const noexcept = 0;
};

class lambertian : public material {
public:
    const color albedo;

    explicit lambertian(const color &a): albedo{a} {}

    [[nodiscard]] bool scatter(
            const ray &r_in,
            const hit_record &rec,
            color &attenuation,
            ray &scattered) const noexcept override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction.
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray{rec.p, scatter_direction};
        attenuation = albedo;
        return true;
    }
};

class metal : public material {
public:
    const color albedo;
    const double fuzz;

    metal(const color &a, double f): albedo{a}, fuzz{f} {}

    [[nodiscard]] bool scatter(
            const ray &r_in,
            const hit_record &rec,
            color &attenuation,
            ray &scattered) const noexcept override {
        const auto reflected = reflect(r_in.direction().unit_vector(), rec.normal);
        scattered = ray{rec.p, reflected + fuzz * random_in_unit_sphere()};
        attenuation = albedo;
        return scattered.direction().dot(rec.normal) > 0;
    }
};
