//  Created by Melih Kurtaran on 30/10/2020.
//  Copyright © 2020 melihkurtaran. All rights reserved.
//

#ifndef ray_h
#define ray_h
#include "vec3.h"

class ray {
    public:
        ray() {}
        ray(const point3& origin, const vec3& direction, double time = 0.0)
            : orig(origin), dir(direction), tm(time)
        {}

        point3 origin() const  { return orig; }
        vec3 direction() const { return dir; }
        double time() const    { return tm; }

        point3 at(double t) const {
            return orig + t*dir;
        }

    public:
        point3 orig;
        vec3 dir;
        double tm;
};

#endif /* ray_h */
