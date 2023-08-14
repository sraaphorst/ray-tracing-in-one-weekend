/**
 * main.cpp
 * By Sebastian Raaphorst, 2023.
 */

#include "rtweekend.h"
#include "aarect.h"
#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "sphere.h"
#include "constant_medium.h"

#include <omp.h>
#include <iostream>
#include <mutex>

[[nodiscard]] color ray_color(const ray &r,
                              const color &background,
                              const hittable &world,
                              int depth) noexcept {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return BLACK;

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 1e-3, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

[[nodiscard]] auto random_scene() noexcept {
    hittable_list world;

    const auto checker = make_shared<checker_texture>(
            color{0.2, 0.3, 0.1},
            color{0.9, 0.9, 0.9}
            );
    const auto ground_material = make_shared<lambertian>(checker);
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
                    const auto center2 = center + vec3{0, random_double(0, 0.5), 0};
                    world.add(make_shared<moving_sphere>(center, center2,
                                                         0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // Metal
                    const auto albedo = color::random(0.5, 1);
                    const auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // Glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    const auto material1 = make_shared<dielectric>(1.5);
    const auto material2 = make_shared<lambertian>(color{0.4, 0.2, 0.1});
    const auto material3 = make_shared<metal>(color{0.7, 0.6, 0.5}, 0.0);

    world.add(make_shared<sphere>(point3{0, 1, 0}, 1.0, material1));
    world.add(make_shared<sphere>(point3{-4, 1, 0}, 1.0, material2));
    world.add(make_shared<sphere>(point3{4, 1, 0}, 1.0, material3));
    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}

hittable_list two_spheres() {
    hittable_list objects;

    const auto texture = make_shared<checker_texture>(
            color{0.2, 0.3, 0.1},
            color(0.9, 0.9, 0.9)
    );
    const auto material = make_shared<lambertian>(texture);
    objects.add(make_shared<sphere>(point3{0, -10, 0}, 10, material));
    objects.add(make_shared<sphere>(point3{0,  10, 0}, 10, material));

    return hittable_list(make_shared<bvh_node>(objects));
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    const auto texture = make_shared<noise_texture>(4);
    const auto material = make_shared<lambertian>(texture);
    objects.add(make_shared<sphere>(point3{0, -1000, 0}, 1000, material));
    objects.add(make_shared<sphere>(point3{0, 2, 0}, 2, material));

    return hittable_list(make_shared<bvh_node>(objects));
}

hittable_list earth() {
    const auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    const auto earth_material = make_shared<lambertian>(earth_texture);
    const auto globe = make_shared<sphere>(point3{0, 0, 0}, 2, earth_material);
    return hittable_list{globe};
}

hittable_list simple_light() {
    hittable_list objects;

    const auto texture = make_shared<noise_texture>(4);
    const auto material = make_shared<lambertian>(texture);
    objects.add(make_shared<sphere>(point3{0, -1000, 0}, 1000, material));
    objects.add(make_shared<sphere>(point3{0, 2, 0}, 2, material));

    const auto difflight = make_shared<diffuse_light>(color{4, 4, 4});
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));
    objects.add(make_shared<sphere>(point3{0, 7, 0}, 2, difflight));

    return hittable_list(make_shared<bvh_node>(objects));
}

hittable_list cornell_box() {
    hittable_list objects;

    const auto red   = make_shared<lambertian>(color{.65, .05, .05});
    const auto white = make_shared<lambertian>(color{.73, .73, .73});
    const auto green = make_shared<lambertian>(color{.12, .45, .15});
    const auto light = make_shared<diffuse_light>(color{15, 15, 15});

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3{0, 0, 0}, point3{165, 330, 165}, white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3{265, 0, 295});
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3{0, 0, 0}, point3{165, 165, 165}, white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3{130, 0, 65});
    objects.add(box2);

    return hittable_list(make_shared<bvh_node>(objects));
}

hittable_list cornell_smoke() {
    hittable_list objects;

    const auto red   = make_shared<lambertian>(color{.65, .05, .05});
    const auto white = make_shared<lambertian>(color{.73, .73, .73});
    const auto green = make_shared<lambertian>(color{.12, .45, .15});
    const auto light = make_shared<diffuse_light>(color{7, 7, 7});

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3{0, 0, 0}, point3{165, 330, 165}, white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3{265, 0, 295});
    objects.add(make_shared<constant_medium>(box1, 0.01, BLACK));

    shared_ptr<hittable> box2 = make_shared<box>(point3{0, 0, 0}, point3{165, 165, 165}, white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3{130, 0, 65});
    objects.add(make_shared<constant_medium>(box2, 0.01, WHITE));

    return hittable_list(make_shared<bvh_node>(objects));
}

int main() {
    std::mutex mtx;

    auto aspect_ratio = 16.0 / 9.0;
    auto image_width = 1000;
//    const auto image_width = 400;
    auto samples_per_pixel = 500;
    const auto max_depth = 50;

    // World
    hittable_list world;
    point3 lookfrom{13, 2, 3};
    point3 lookat{0, 0, 0};
    auto vfov = 20.0; // 40.0
    auto aperture = 0.0;
    color background{0.70, 0.80, 1.00}; // BLACK

    switch (0) {
        case 1:
            world = random_scene();
            aperture = 0.1;
            break;

        case 2:
            world = two_spheres();
            break;

        case 3:
            world = two_perlin_spheres();
            break;

        case 4:
            world = earth();
            break;

        case 5:
            world = simple_light();
            samples_per_pixel = 400;
            background = BLACK;
            lookfrom = point3{26, 3, 6};
            lookat = point3{0, 2, 0};
            break;

        case 6:
            world = cornell_box();
            aspect_ratio = 1.0;
            image_width = 600;
            samples_per_pixel = 200;
            background = BLACK;
            lookfrom = point3{278, 278, -800};
            lookat = point3{278, 278, 0};
            vfov = 40.0;
            break;

        default:
        case 7:
            world = cornell_smoke();
            aspect_ratio = 1.0;
            image_width = 600;
            samples_per_pixel = 200;
            lookfrom = point3{278, 278, -800};
            lookat = point3{278, 278, 0};
            vfov = 40.0;
            break;
    }

    const auto image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera
    const vec3 vup{0, 1, 0};
    const auto dist_to_focus = 10.0;
    camera cam(lookfrom, lookat, vup, vfov,
               aspect_ratio, aperture, dist_to_focus,
               0.0, 1.0);

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (auto j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        std::vector<color> line(image_width);
        #pragma omp parallel for schedule(dynamic) default(none) shared(background, mtx, std::cout, j, world, cam, image_width, image_height, max_depth, samples_per_pixel, line)
        for (auto i = 0; i < image_width; ++i) {
            color pixel_color{0, 0, 0};
            for (int s = 0; s < samples_per_pixel; ++s) {
                const auto u = (i + random_double()) / (image_width - 1);
                const auto v = (j + random_double()) / (image_height - 1);
                const auto r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            {
                std::lock_guard<std::mutex> lock(mtx);
                line[i] = pixel_color;
            }
        }

        for (const auto pixel_color: line)
            write_color(std::cout, pixel_color, samples_per_pixel);
    }

    std::cerr << "\nDone.\n";
}
