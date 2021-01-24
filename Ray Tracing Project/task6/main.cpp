//  Created by Melih Kurtaran on 29/10/2020.
//  Copyright © 2020 melihkurtaran. All rights reserved.

#include <iostream>
#include <fstream>
#include "ray.h"
#include "vec3.h"
#include "color.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "camera.h"
#include <cstdlib>
#include "material.h"

using namespace std;

// Constants

const double INF = numeric_limits<double>::infinity();

hittable_list simple_light() {
    hittable_list objects;

    auto material = make_shared<lambertian>(color(0.2, 0.2, 0.7));
    auto redlight = make_shared<diffuse_light>(color(1,0,0));
    auto bluelight = make_shared<diffuse_light>(color(0,0,1));
    auto greenlight = make_shared<diffuse_light>(color(0,1,0));
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, material));
    objects.add(make_shared<sphere>(point3(-3, 2, 2), 2, redlight));
    objects.add(make_shared<sphere>(point3(3, 2, 2), 2, greenlight));
    objects.add(make_shared<sphere>(point3(0, 6, 6), 2, bluelight));
    objects.add(make_shared<sphere>(point3(0, 2, 2), 2, material));

    return objects;
}

hittable_list two_spheres() {
    hittable_list objects;

    auto material = make_shared<lambertian>(color(0.2, 0.2, 0.7));
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, material));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, material));

    return objects;
}

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, INF, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.7, 0.2, 0.3));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<diffuse_light>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<lambertian>(color(0.2, 0.2, 0.7));
    world.add(make_shared<sphere>(point3(-5, 1.5, 0), 1.5, material1));

    auto material2 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(-1, 1.5, 0), 1.5, material2));

    auto material3 = make_shared<dielectric>(0.9);
    world.add(make_shared<sphere>(point3(2, 1, 0), 1, material3));
    auto material4 = make_shared<dielectric>(1.1);
    world.add(make_shared<sphere>(point3(5, 1, 0), 1, material4));
    auto material5 = make_shared<dielectric>(2.5);
    world.add(make_shared<sphere>(point3(8, 1, 0), 1, material5));
    
    return world;
}


int main() {
    
    ofstream img("image.ppm");

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    
    // World
    
    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background(0,0,0);

    switch (1) {
        case 1:
            world = random_scene();
            background = color(0.0, 0.0, 0.0);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            aperture = 0.1;
            break;

        default:
        case 2:
            world = simple_light();
            background = color(0,0,0);
            lookfrom = point3(26,3,6);
            lookat = point3(0,2,0);
            vfov = 20.0;
            break;
            break;
    }

    // Camera

    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    
    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    img << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
//            write_color(cout, pixel_color, samples_per_pixel);
            write_color(img, pixel_color, samples_per_pixel);

        }
    }
    std::cerr << "\nDone.\n";
    system("open image.ppm");
}
