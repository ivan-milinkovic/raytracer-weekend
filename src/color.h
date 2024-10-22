#ifndef color_h
#define color_h

#include <iostream>
#include "vec3.h"


inline double clamp_color_comp(double n) {
    if (n < 0) return 0;
    if (n > 0.999) return 0.999;
    return n;
}

inline void write_color(std::ostream& stream, const Vec3& color) {
    auto r = color.X();
    auto g = color.Y();
    auto b = color.Z();

    // Translate the [0,1] component values to the byte range [0,255].
    int rbyte = int(256 * clamp_color_comp(r));
    int gbyte = int(256 * clamp_color_comp(g));
    int bbyte = int(256 * clamp_color_comp(b));

    // Write out the pixel color components.
    stream << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

inline double linear_to_gamma(double linear_value) {
    // inverse gamma 2
    if (linear_value < 0)
        return 0;
    return std::sqrt(linear_value);
}

inline void gamma_correct(Vec3& color) {
    color.v[0] = linear_to_gamma(color.v[0]);
    color.v[1] = linear_to_gamma(color.v[1]);
    color.v[2] = linear_to_gamma(color.v[2]);
}

#endif /* color_h */
