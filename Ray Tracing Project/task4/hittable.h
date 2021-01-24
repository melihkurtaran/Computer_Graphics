//  Created by Melih Kurtaran on 30/10/2020.
//  Copyright © 2020 melihkurtaran. All rights reserved.
//

#ifndef hittable_h
#define hittable_h

#include "ray.h"
#include "material.h"
#include "vec3.h"

class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};



#endif /* hittable_h */
