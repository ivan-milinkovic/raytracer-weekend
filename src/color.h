#ifndef color_h
#define color_h

#include <iostream>
#include "Vec3.h"

void write_color(std::ostream& out, const Vec3& color) {
    auto r = color.X();
    auto g = color.Y();
    auto b = color.Z();

    // Translate the [0,1] component values to the byte range [0,255].
    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}


#endif /* color_h */
