/**
 * aarect.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once


#include <utility>

#include "hittable.h"

class xy_rect final : public hittable {
public:
    double x0, x1, y0, y1, k;
    shared_ptr<material> mat;

    xy_rect(double x0, double x1, double y0, double y1, double k, shared_ptr<material> mat) noexcept
            : x0{x0}, x1{x1}, y0{y0}, y1{y1}, k{k}, mat{std::move(mat)} {}

    [[nodiscard]] bool hit(const ray &r,
                           double t_min,
                           double t_max,
                           hit_record &rec) const noexcept override {
        const auto t = (k - r.origin().z()) / r.direction().z();
        if (t < t_min || t > t_max)
            return false;

        const auto x = r.origin().x() + t * r.direction().x();
        const auto y = r.origin().y() + t * r.direction().y();
        if (x < x0 || x > x1 || y < y0 || y > y1)
            return false;

        rec.u = (x - x0) / (x1 - x0);
        rec.v = (y - y0) / (y1 - y0);
        rec.t = t;

        const vec3 outward_normal{0, 0, 1};
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat;
        rec.p = r.at(t);

        return true;
    }

    [[nodiscard]] bool bounding_box(double time0, double time1, aabb &output_box) const noexcept override {
        // Must have non-zero width in each dimension, so pad z with a small amount.
        output_box = aabb{point3{x0, y0, k - 1e-4}, point3{x1, y1, k + 1e-4}};
        return true;
    }
};

class xz_rect final : public hittable {
public:
    double x0, x1, z0, z1, k;
    shared_ptr<material> mat;

    xz_rect(double x0, double x1, double z0, double z1, double k, shared_ptr<material> mat) noexcept
    : x0{x0}, x1{x1}, z0{z0}, z1{z1}, k{k}, mat{std::move(mat)} {}

    [[nodiscard]] bool hit(const ray &r,
                           double t_min,
                           double t_max,
                           hit_record &rec) const noexcept override {
        const auto t = (k - r.origin().y()) / r.direction().y();
        if (t < t_min || t > t_max)
            return false;

        const auto x = r.origin().x() + t * r.direction().x();
        const auto z = r.origin().z() + t * r.direction().z();
        if (x < x0 || x > x1 || z < z0 || z > z1)
            return false;

        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;

        const vec3 outward_normal{0, 1, 0};
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat;
        rec.p = r.at(t);

        return true;
    }

    [[nodiscard]] bool bounding_box(double time0, double time1, aabb &output_box) const noexcept override {
        // Must have non-zero width in each dimension, so pad z with a small amount.
        output_box = aabb{point3{x0, k - 1e-4, z0}, point3{x1, k + 1e-4, z1}};
        return true;
    }
};

class yz_rect final : public hittable {
public:
    double y0, y1, z0, z1, k;
    shared_ptr<material> mat;

    yz_rect(double y0, double y1, double z0, double z1, double k, shared_ptr<material> mat) noexcept
            : y0{y0}, y1{y1}, z0{z0}, z1{z1}, k{k}, mat{std::move(mat)} {}

    [[nodiscard]] bool hit(const ray &r,
                           double t_min,
                           double t_max,
                           hit_record &rec) const noexcept override {
        const auto t = (k - r.origin().x()) / r.direction().x();
        if (t < t_min || t > t_max)
            return false;

        const auto y = r.origin().y() + t * r.direction().y();
        const auto z = r.origin().z() + t * r.direction().z();
        if (y < y0 || y > y1 || z < z0 || z > z1)
            return false;

        rec.u = (y - y0) / (y1 - y0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;

        const vec3 outward_normal{1, 0, 0};
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat;
        rec.p = r.at(t);

        return true;
    }

    [[nodiscard]] bool bounding_box(double time0, double time1, aabb &output_box) const noexcept override {
        // Must have non-zero width in each dimension, so pad z with a small amount.
        output_box = aabb{point3{k - 1e-4, y0, z0}, point3{k + 1e-4, y1, z1}};
        return true;
    }
};
