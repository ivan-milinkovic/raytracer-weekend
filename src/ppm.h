#ifndef MYPPM
#define MYPPM

#include "Vec3.h"
#include "color.h"

void print_ppm(const Vec3 * rgb, int w, int h)
{
    std::cout << "P3\n";
    std::cout << w << ' ' << h << '\n';
    std::cout << 255 << '\n';

    for (int r = 0; r < h; r++)
    {
        for (int c = 0; c < w; c++)
        {
            int i = r * w + c;
            auto color = rgb[i];
            write_color(std::cout, color);
        }
    }
    std::cout << '\n';
}

#endif
