#ifndef util_h
#define util_h

#include <limits>
#include <filesystem>
#include <cstdlib>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

const double deg_to_rad = pi / 180.0;

inline double rad_from_deg(double degrees) {
    return degrees * deg_to_rad;
}

inline std::filesystem::path out_dir() {
    auto path = std::filesystem::path(__FILE__);
    path.remove_filename();
    return path;
}

// [0,1)
inline double rw_random() {
    return std::rand() / (RAND_MAX + 1.0);
}

// [min,max)
inline double rw_random(double min, double max) {
    return min + (max - min) * rw_random();
}

#endif /* util_h */
