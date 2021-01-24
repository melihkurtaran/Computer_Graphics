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
#include <random>

using namespace std;

// Constants

const double INF = numeric_limits<double>::infinity();
//const double PI = 3.1415926535897932385;


inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}


color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, 0, INF, rec)) {
        return 0.5 * (rec.normal + color(1,1,1));
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
    
    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    
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
                pixel_color += ray_color(r, world);
            }
            write_color(cout, pixel_color, samples_per_pixel);
            write_color(img, pixel_color, samples_per_pixel);

        }
    }
    std::cerr << "\nDone.\n";
    system("open image.ppm");
}
