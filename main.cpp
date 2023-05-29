/**
 * main.cpp
 * By Sebastian Raaphorst, 2023.
 */

#include "rtweekend.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>

[[nodiscard]] auto ray_color(const ray &r, const hittable &world, int depth) noexcept {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color{0, 0, 0};

    hit_record rec;
    if (world.hit(r, 1e-3, infinity, rec)) {
        const auto target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5 * ray_color(ray{rec.p, target - rec.p}, world, depth - 1);
    }

    const auto unit_direction = r.direction().unit_vector();
    const auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color{1.0, 1.0, 1.0} + t * color{0.5, 0.7, 1.0};
}

int main() {
    const auto aspect_ratio = 16.0 / 9.0;
    const auto image_width = 400;
    const auto image_height = static_cast<int>(image_width / aspect_ratio);
    const auto samples_per_pixel = 100;
    const auto max_depth = 50;

    hittable_list world;
    world.add(make_shared<sphere>(point3{0, 0, -1}, 0.5));
    world.add(make_shared<sphere>(point3{0, -100.5, -1}, 100));

    camera cam;

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
