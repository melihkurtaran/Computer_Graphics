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

using namespace std;

// Constants

const double INF = numeric_limits<double>::infinity();
//const double PI = 3.1415926535897932385;

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
    
    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0.4,-1.4), 0.3));
    world.add(make_shared<sphere>(point3(0,0.1,-1), 0.2));
    world.add(make_shared<sphere>(point3(0,0,-0.6), 0.1));
    
    world.add(make_shared<sphere>(point3(0.8,0.4,-1.4), 0.3));
    world.add(make_shared<sphere>(point3(0.8,0.1,-1), 0.2));
    world.add(make_shared<sphere>(point3(0.8,-0.2,-0.9), 0.1));
    
    world.add(make_shared<sphere>(point3(-0.8,0.4,-1.4), 0.3));
    world.add(make_shared<sphere>(point3(-0.8,0.1,-1), 0.2));
    world.add(make_shared<sphere>(point3(-0.8,-0.2,-0.6), 0.1));
    
    // Camera
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(-0.4,0, -2.5);
//    auto origin = point3(0,0,0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);
    
    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    img << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width-1);
            auto v = double(j) / (image_height-1);
            ray r(origin, -1*(lower_left_corner + u*horizontal + v*vertical - origin));
            color pixel_color = ray_color(r, world);

            write_color(std::cout, pixel_color);
            write_color(img, pixel_color);
        }
    }
    std::cerr << "\nDone.\n";
    system("open image.ppm");
}
