#ifndef util_h
#define util_h

#include <limits>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

double deg_to_rad = pi / 180.0;

inline double rad_from_deg(double degrees) {
    return degrees * deg_to_rad;
}

std::filesystem::path out_dir() {
    auto path = std::filesystem::path(__FILE__);
    path.remove_filename();
    return path;
}

#endif /* util_h */
