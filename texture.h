/**
 * texture.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include <utility>

#include "vec3.h"

class texture {
public:
    [[nodiscard]] virtual color value(double u, double v, const point3 &p) const noexcept = 0;
};

class solid_color final : public texture {
private:
    color color_value;

public:
    solid_color() noexcept = default;
    explicit solid_color(color c) noexcept: color_value{c} {}
    solid_color(double red, double green, double blue) noexcept: solid_color{color{red, green, blue}} {}

    [[nodiscard]] color value(double u, double v, const point3 &p) const noexcept override {
        return color_value;
    }
};

class checker_texture final : public texture {
public:
    std::shared_ptr<texture> odd;
    std::shared_ptr<texture> even;

    checker_texture() noexcept = default;

    checker_texture(std::shared_ptr<texture> even, std::shared_ptr<texture> odd) noexcept
    : even{std::move(even)}, odd{std::move(odd)} {}

    checker_texture(color even_color, color odd_color) noexcept
            : checker_texture(make_shared<solid_color>(even_color), make_shared<solid_color>(odd_color)) {}

    [[nodiscard]] color value(double u, double v, const point3 &p) const noexcept override {
        const auto sines = std::sin(10 * p.x()) * std::sin(10 * p.y()) * std::sin(10 * p.z());
        return sines < 0 ? odd->value(u, v, p) : even->value(u, v, p);
    }
};
