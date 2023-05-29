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

[[nodiscard]] auto random_scene() noexcept {
    hittable_list world;

    const auto ground_material = make_shared<lambertian>(color{0.5, 0.5, 0.5});
    world.add(make_shared<sphere>(point3{0, -1000, 0}, 1000, ground_material));

    for (auto a = -11; a < 11; ++a) {
        for (auto b = -11; b < 11; ++b) {
            const point3 center{a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};

            if ((center - point3{4, 0.2, 0}).length() > 0.9) {
                const auto choose_mat = random_double();
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // Diffuse.
                    const auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                } else if (choose_mat < 0.95) {
                    // Metal
                    const auto albedo = color::random(0.5, 1);
                    const auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                } else {
                    // Glass
                    sphere_material = make_shared<dielectric>(1.5);
                }
                world.add(make_shared<sphere>(center, 0.2, sphere_material));
            }
        }
    }

    const auto material1 = make_shared<dielectric>(1.5);
    const auto material2 = make_shared<lambertian>(color{0.4, 0.2, 0.1});
    const auto material3 = make_shared<metal>(color{0.7, 0.6, 0.5}, 0.0);

    world.add(make_shared<sphere>(point3{0, 1, 0}, 1.0, material1));
    world.add(make_shared<sphere>(point3{-4, 1, 0}, 1.0, material2));
    world.add(make_shared<sphere>(point3{4, 1, 0}, 1.0, material3));

    return world;
}

int main() {
    const auto aspect_ratio = 3.0 / 2.0;
    const auto image_width = 1200;
    const auto image_height = static_cast<int>(image_width / aspect_ratio);
    const auto samples_per_pixel = 500;
    const auto max_depth = 50;

    const auto world = random_scene();

    const point3 lookfrom{13, 2, 3};
    const point3 lookat{0, 0, 0};
    const vec3 vup{0, 1, 0};
    const auto dist_to_focus = 10.0;
    const auto aperture = 0.1;

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
