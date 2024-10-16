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
        
        return intersect(ray, limits, hit);
        
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
    
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
    
    inline bool intersect(const Ray &ray, Interval limits, Hit& hit) const
    {
        double t0, t1; // solutions for t if the ray intersects
#if 1
        // Geometric solution
        Vec3 L = center - ray.Origin();
        double tca = dot(L, ray.Dir());
        // if (tca < 0) return false;
        double d2 = dot(L, L) - tca * tca;
        if (d2 > r*r) return false;
        double thc = sqrt(r*r - d2);
        t0 = tca - thc;
        t1 = tca + thc;
#else
        // Analytic solution
        Vec3 L = ray.Origin() - center;
        double a = dot(ray.Dir(), ray.Dir());
        double b = 2 * dot(ray.Dir(), L);
        double c = dot(L,L) - r*r;
        if (!solveQuadratic(a, b, c, t0, t1)) return false;
#endif
        double d = t0;
        if (!limits.surrounds(t0)) {
            d = t1;
            if (!limits.surrounds(d))
                return false;
        }

        if (!limits.surrounds(d)) {
            return false;
        }
        
        hit.d = d;
        hit.p = ray.at(hit.d);
        hit.n = (hit.p - center) / r;
        hit.is_front = dot(ray.Dir(), hit.n) < 0;
        hit.n = hit.is_front ? hit.n : -1 * hit.n;
        hit.material = material;

        return true;
    }
    
    inline bool solveQuadratic(const double &a, const double &b, const double &c, double &x0, double &x1) const
    {
        double discr = b * b - 4 * a * c;
        if (discr < 0) return false;
        else if (discr == 0) x0 = x1 = - 0.5 * b / a;
        else {
            float q = (b > 0) ?
                -0.5 * (b + sqrt(discr)) :
                -0.5 * (b - sqrt(discr));
            x0 = q / a;
            x1 = c / q;
        }
        if (x0 > x1) std::swap(x0, x1);
        
        return true;
    }
};


#endif /* geometry_h */
