#ifndef geometry_h
#define geometry_h

#include "ray.h"
#include "vec3.h"
#include "interval.h"
class Material;

class Hit {
public:
    Vec3 p;
    Vec3 n;
    double d; // distance from ray origin
    
    Material* material;
    bool is_front;
};

class Sphere {
public:
    Vec3 center;
    double r;
    Material* material;
    
    Sphere(Vec3 center, double r, Material* material): center(center), r(r), material(material) { }
    
    bool hit(const Ray& ray, Interval limits, Hit& hit) {
        Vec3 OC = center - ray.Origin();
        double a = ray.Dir().len_sq();
        double h = dot(ray.Dir(), OC);
        double c = OC.len_sq() - r*r;

        double dcr = h*h - a*c; // discriminant
        if (dcr < 0)
            return false; // no solution / roots
        
        auto sqrtd = std::sqrt(dcr);
        auto root = (h - sqrtd) / a; // closer, smaller root
        if (!limits.surrounds(root)) {
            root = (h + sqrtd) / a; // the other root
            if (!limits.surrounds(root))
                return false;
        }

        hit.d = root;
        hit.p = ray.at(hit.d);
        hit.n = (hit.p - center) / r;
        hit.is_front = dot(ray.Dir(), hit.n) < 0;
        hit.n = hit.is_front ? hit.n : -1 * hit.n;
        hit.material = material;
        
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
