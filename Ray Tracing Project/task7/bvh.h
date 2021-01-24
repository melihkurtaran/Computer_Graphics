//  Created by Melih Kurtaran on 31/10/2020.
//  Copyright © 2020 melihkurtaran. All rights reserved.
//

#ifndef bvh_h
#define bvh_h

#include "hittable.h"
#include "hittable_list.h"
#include <random>
#include "aabb.h"

#include <algorithm>



class bvh_node : public hittable  {
    public:
        bvh_node();

        bvh_node(const hittable_list& list, double time0, double time1)
            : bvh_node(list.objects, 0, list.objects.size(), time0, time1)
        {}

        bvh_node(
            const std::vector<shared_ptr<hittable>>& src_objects,
            size_t start, size_t end, double time0, double time1);

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        shared_ptr<hittable> left;
        shared_ptr<hittable> right;
        aabb box;
};


bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}




#endif /* bvh_h */

