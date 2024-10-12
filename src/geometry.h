#ifndef geometry_h
#define geometry_h

#include "Ray.h"

double ray_to_sphere_distance(Ray ray, Vec3 C, double r) {
    Vec3 OC = C - ray.Origin();
    double a = dot(ray.Dir(), ray.Dir());
    double b = -2 * dot(ray.Dir(), OC);
    double c = dot(OC, OC) - r*r;
    double discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return -1;
    } else {
        return -b - std::sqrt(discriminant);
    }
}

#endif /* geometry_h */
