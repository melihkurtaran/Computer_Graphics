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

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, INF, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
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
//    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
//    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));
    auto yellowBall = make_shared<lambertian>(color(0.8,0.8,0));
    auto purpleBall= make_shared<lambertian>(color(0.2, 0.1, 0.6));
    auto mirrorBall   = make_shared<metal>(color(0.5, 0.5, 0.5), 0);
    auto greenMirrorBall  = make_shared<metal>(color(0.2, 0.6, 0.2), 0);
    auto redMirrorBall  = make_shared<dielectric>(0.5);
    auto iceBall = make_shared<dielectric>(1.31);
    auto diamondBall = make_shared<dielectric>(2.417);
    auto airBall = make_shared<dielectric>(1.01);
    auto refBall = make_shared<dielectric>(10);
    
    world.add(make_shared<sphere>(point3(-3.0,0,-3.0), 0.5, purpleBall));
    world.add(make_shared<sphere>(point3(-2.0,0,-3.0), 0.5, iceBall));
    world.add(make_shared<sphere>(point3(-1.0,0,-3.0), 0.5, yellowBall));
    world.add(make_shared<sphere>(point3(0,0,-3.0), 0.5, diamondBall));
    world.add(make_shared<sphere>(point3(1.0,0,-3.0), 0.5, greenMirrorBall));
    world.add(make_shared<sphere>(point3(2.0,0,-3.0), 0.5, airBall));
    world.add(make_shared<sphere>(point3(3.0,0,-3.0), 0.5, purpleBall));
    world.add(make_shared<sphere>(point3(-1.0,1.0,-3.0), 0.5, refBall));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100,diamondBall));
    // Camera
    camera cam;
    
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
                pixel_color += ray_color(r, world, max_depth);
            }
//            write_color(cout, pixel_color, samples_per_pixel);
            write_color(img, pixel_color, samples_per_pixel);

        }
    }
    std::cerr << "\nDone.\n";
    system("open image.ppm");
}
