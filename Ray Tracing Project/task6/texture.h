//  Created by Melih Kurtaran on 31/10/2020.
//  Copyright © 2020 melihkurtaran. All rights reserved.
//

#ifndef texture_h
#define texture_h

class texture {
    public:
        virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
    public:
        solid_color() {}
        solid_color(color c) : color_value(c) {}

        solid_color(double red, double green, double blue)
          : solid_color(color(red,green,blue)) {}

        virtual color value(double u, double v, const vec3& p) const override {
            return color_value;
        }

    private:
        color color_value;
};


#endif /* texture_h */
