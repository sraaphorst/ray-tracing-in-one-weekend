/**
 * main.cpp
 * By Sebastian Raaphorst, 2023.
 */

#include "rtweekend.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

#include <iostream>

[[nodiscard]] auto ray_color(const ray &r, const hittable &world, int depth) noexcept {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return BLACK;

    hit_record rec;
    if (world.hit(r, 1e-3, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return BLACK;
    }

    const auto unit_direction = r.direction().unit_vector();
    const auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * WHITE + t * SKY_BLUE;
}

int main() {
    const auto aspect_ratio = 16.0 / 9.0;
    const auto image_width = 400;
    const auto image_height = static_cast<int>(image_width / aspect_ratio);
    const auto samples_per_pixel = 100;
    const auto max_depth = 50;

    const auto R = std::cos(M_PI_4);
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0), -0.45, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    const point3 lookfrom{3, 3, 2};
    const point3 lookat{0, 0, -1};
    const vec3 vup{0, 1, 0};
    const auto dist_to_focus = (lookfrom - lookat).length();
    const auto aperture = 2.0;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (auto j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (auto i = 0; i < image_width; ++i) {
            color pixel_color{0, 0, 0};
            for (int s = 0; s < samples_per_pixel; ++s) {
                const auto u = (i + random_double()) / (image_width - 1);
                const auto v = (j + random_double()) / (image_height - 1);
                const auto r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
