/**
 * texture.h
 * By Sebastian Raaphorst, 2023.
 */

#pragma once

#include <utility>

#include "perlin.h"
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

class noise_texture final : public texture {
public:
    perlin noise;
    const double scale;

    explicit noise_texture(double scale) noexcept: scale{scale} {}

    [[nodiscard]] color value(double u, double v, const point3 &p) const noexcept override {
//        return GREY * (1.0 + noise.noise(scale * p));
//        return WHITE * noise.turb(scale * p);
        return GREY * (1 + std::sin(scale * p.z() + 10 * noise.turb(p)));
    }
};

class image_texture final : public texture {
private:
    unsigned char *data;
    int width, height;
    int bytes_per_scanline;

public:
    const static int bytes_per_pixel = 3;
    const static color default_color;
    const static double color_scale;

    image_texture() noexcept
    : data{nullptr}, width{0}, height{0}, bytes_per_scanline{0} {}

    explicit image_texture(const std::string& filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(filename.c_str(), &width, &height, &components_per_pixel, components_per_pixel);
        if (!data) {
            std::cerr << "ERROR: Could not load texture image file: '" << filename << "'\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~image_texture() {
        delete[] data;
    }

    [[nodiscard]] color value(double u, double v, const vec3 &p) const noexcept override {
        if (data == nullptr)
            return default_color;

        // Clamp input texture coordinates to [0,1] x [0,1].
        // We flip v to image coordinates.
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since the actual coordinates should be < 1.0
        if (i >= width) i = width - 1;
        if (j >= height) j = height - 1;

        const auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;
        return color{color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]};
    }
};

const color image_texture::default_color{0, 1, 1};
const double image_texture::color_scale = 1.0 / 255.0;