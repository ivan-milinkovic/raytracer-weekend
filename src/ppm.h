#ifndef MYPPM
#define MYPPM

#include <filesystem>
#include <fstream>
#include "vec3.h"
#include "color.h"
#include "util.h"

std::filesystem::path ppm_file_path() {
    auto path = out_dir();
    path = path / "..";
    path /= "out.ppm";
    return path;
}

void write_ppm_file(const Vec3 * rgb, int w, int h)
{
    auto ppm_path = ppm_file_path();
    std::ofstream file(ppm_path);
    std::cout << "Writing to:\n" << ppm_path << std::endl;
    // std::ostream& file = std::cout;
    
    file << "P3\n";
    file << w << ' ' << h << '\n';
    file << 255 << '\n';

    for (int r = 0; r < h; r++)
    {
        for (int c = 0; c < w; c++)
        {
            int i = r * w + c;
            auto color = rgb[i];
            write_color(file, color);
        }
    }
    file << '\n';
    file.close();
}

#endif
