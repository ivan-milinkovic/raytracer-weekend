#ifndef geometry_h
#define geometry_h

#include "ray.h"
#include "vec3.h"

class Hit {
public:
    Vec3 p;
    Vec3 n;
    double d; // distance from ray origin
    bool is_front;
};

class Sphere {
public:
    Vec3 center;
    double r;
    
    Sphere(Vec3 center, double r): center(center), r(r) { }
    
    bool hit(const Ray& ray, double d_min, double d_max, Hit& hit) {
        Vec3 OC = center - ray.Origin();
        double a = ray.Dir().len_sq();
        double h = dot(ray.Dir(), OC);
        double c = OC.len_sq() - r*r;

        double dcr = h*h - a*c; // discriminant
        if (dcr < 0)
            return false; // no solution / roots
        
        auto sqrtd = std::sqrt(dcr);
        auto root = (h - sqrtd) / a; // closer, smaller root
        if (root <= d_min || d_max <= root) {
            root = (h + sqrtd) / a; // the other root
            if (root <= d_min || d_max <= root)
                return false;
        }

        hit.d = root;
        hit.p = ray.at(hit.d);
        hit.n = (hit.p - center) / r;
        hit.is_front = dot(ray.Dir(), hit.n) < 0;
        hit.n = hit.is_front ? hit.n : -1 * hit.n;
        
        return true;
    }
};



double ray_to_sphere_distance(Ray ray, Vec3 C, double r) {
    Vec3 OC = C - ray.Origin();
    double a = ray.Dir().len_sq();
    double h = dot(ray.Dir(), OC);
    double c = OC.len_sq() - r*r;
    double discriminant = h*h - a*c;
    if (discriminant < 0) {
        return -1;
    } else {
        return (h - std::sqrt(discriminant)) / a;
    }
}

//double ray_to_sphere_distance0(Ray ray, Vec3 C, double r) {
//    Vec3 OC = C - ray.Origin();
//    double a = dot(ray.Dir(), ray.Dir());
//    double b = -2 * dot(ray.Dir(), OC);
//    double c = dot(OC, OC) - r*r;
//    double discriminant = b*b - 4*a*c;
//    if (discriminant < 0) {
//        return -1;
//    } else {
//        return (-b - std::sqrt(discriminant)) / (2*a);
//    }
//}

#endif /* geometry_h */
