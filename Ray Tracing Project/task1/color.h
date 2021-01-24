//  Created by Melih Kurtaran on 30/10/2020.
//  Copyright © 2020 melihkurtaran. All rights reserved.
//

#ifndef color_h
#define color_h
#include "vec3.h"

#include <iostream>

void write_color(std::ostream &out, color pixel_color) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

#endif /* color_h */
