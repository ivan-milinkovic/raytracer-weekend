#ifndef color_h
#define color_h

#include <iostream>
#include "vec3.h"
#include "interval.h"

void write_color(std::ostream& stream, const Vec3& color) {
    auto r = color.X();
    auto g = color.Y();
    auto b = color.Z();

    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval limits(0.0, 0.999);
    int rbyte = int(256 * limits.clamp(r));
    int gbyte = int(256 * limits.clamp(g));
    int bbyte = int(256 * limits.clamp(b));

    // Write out the pixel color components.
    stream << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}


#endif /* color_h */
