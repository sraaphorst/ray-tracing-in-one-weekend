/**
 * color.h
 * By Sebastian Raaphorst, 2023.
 */
#pragma once

#include "vec3.h"

#include <iostream>

[[nodiscard]] auto color_int(double c) noexcept {
    return static_cast<int>(256 * clamp(c, 0.0, 0.999));
}

void write_color(std::ostream &out, const color &pixel_color, int samples_per_pixel) noexcept {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma-correct for gamma = 2.0.
    const auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(color_int(r)) << ' '
        << static_cast<int>(color_int(g)) << ' '
        << static_cast<int>(color_int(b)) << '\n';
}
