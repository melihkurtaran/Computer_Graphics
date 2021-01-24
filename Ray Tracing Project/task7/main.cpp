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
#include "box.h"

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

hittable_list pyramid() {
    hittable_list world;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<box>(point3(-15,-3,-15),point3(15,0,15),ground_material));
    
    for(double k=0.2;k<3;k+=0.4)
    {
        for(double i=-3+k;i<3-k;i+=0.4)
        {
            for(double j=-3+k;j<3-k;)
            {
                j+=0.4;
                point3 center(i,k,j);
                auto randomColor = color::random() * color::random();
                shared_ptr<material> material = make_shared<lambertian>(randomColor);
                world.add(make_shared<box>(point3(center.x()-0.2,center.y()-0.2,center.z()-0.2),point3(center.x()+0.2,center.y()+0.2,center.z()+0.2),material));
            }
        }
    }
    
    
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
    hittable_list world = pyramid();
    
    // Camera
    point3 lookfrom(6,10,12);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);

    camera cam(lookfrom,lookat, vup, 20, aspect_ratio);
    
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
