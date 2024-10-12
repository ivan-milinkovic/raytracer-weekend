#ifndef geometry_h
#define geometry_h

#include "Ray.h"

bool ray_sphere_hit(Ray ray, Vec3 C, double r) {
    Vec3 OC = C - ray.Origin();
    double a = dot(ray.Dir(), ray.Dir());
    double b = -2 * dot(ray.Dir(), OC);
    double c = dot(OC, OC) - r*r;
    double discriminant = b*b - 4*a*c;
    return discriminant >= 0;
}

#endif /* geometry_h */
