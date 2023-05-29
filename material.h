/**
 * material.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include "ray.h"

struct hit_record;

class material {
public:
    virtual bool scatter(
            const ray &r_in,
            const hit_record &rec,
            color &attenuation,
            ray &scattered) const noexcept = 0;
};
