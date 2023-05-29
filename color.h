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
    const auto scale = 1.0 / samples_per_pixel;

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(color_int(pixel_color.x() * scale)) << ' '
        << static_cast<int>(color_int(pixel_color.y() * scale)) << ' '
        << static_cast<int>(color_int(pixel_color.z() * scale)) << '\n';
}
