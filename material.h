/**
 * material.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include <utility>

#include "ray.h"
#include "hittable.h"
#include "texture.h"

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
    std::shared_ptr<texture> albedo;

    explicit lambertian(const color &a) noexcept: albedo{make_shared<solid_color>(a)} {}
    explicit lambertian(std::shared_ptr<texture> a) noexcept: albedo{std::move(a)} {}

    [[nodiscard]] bool scatter(
            const ray &r_in,
            const hit_record &rec,
            color &attenuation,
            ray &scattered) const noexcept override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction.
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray{rec.p, scatter_direction, r_in.time()};
        attenuation = albedo->value(rec.u, rec.v, rec.p);
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
        scattered = ray{rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time()};
        attenuation = albedo;
        return scattered.direction().dot(rec.normal) > 0;
    }
};

class dielectric : public material {
public:
    // Index of refraction.
    const double ir;

    explicit dielectric(double index_of_refraction): ir{index_of_refraction} {}

    [[nodiscard]] bool scatter(
            const ray &r_in,
            const hit_record &rec,
            color &attenuation,
            ray &scattered) const noexcept override {
        attenuation = WHITE;
        const auto refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        const auto unit_direction = r_in.direction().unit_vector();
        const auto cos_theta = std::min((-unit_direction).dot(rec.normal), 1.0);
        const auto sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        const auto cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray{rec.p, direction, r_in.time()};
        return true;
    }

private:
    [[nodiscard]] static double reflectance(double cosine, double ref_idx) noexcept {
        // Schlick's approximation.
        const auto r0 = (1 - ref_idx) / (1 + ref_idx);
        const auto r0_2 = r0 * r0;
        return r0_2 + (1 - r0_2) * std::pow(1 - cosine, 5);
    }
};
