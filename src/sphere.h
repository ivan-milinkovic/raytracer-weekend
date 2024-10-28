#ifndef geometry_h
#define geometry_h

#include <memory>
#include "ray.h"
#include "vec3.h"
#include "interval.h"
#include "aabb.h"
#include "hittable.h"

// 0 - original RT weekend, 1 - scratch-a-pixel geometric (fastest), 2 - scratch-a-pixel analytic
#define HIT_IMPL 1
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html


class Sphere: public Hittable { // clang crashes without public inheritance
public:
    Vec3 center;
    double r;
    std::shared_ptr<Material> material;
    Vec3 center2; // next frame center
    Vec3 velocity;
    AABB bbox;
    
    Sphere(const Vec3& center, double r, std::shared_ptr<Material> material)
    : center(center), r(r), material(material),
      Hittable(HittableType_Sphere)
    {
        Vec3 rv = Vec3(r,r,r);
        bbox = AABB(center - rv, center + rv);
    }
    
    Sphere(const Vec3& center, const Vec3& center2, double r, std::shared_ptr<Material> material)
    : center(center), r(r), material(material), center2(center2),
      Hittable(HittableType_Sphere)
    {
        velocity = center2 - center;
        Vec3 rv = Vec3(r,r,r);
        AABB box0 = AABB(center - rv, center + rv);
        AABB box1 = AABB(center2 - rv, center2 + rv);
        bbox = AABB(box0, box1);
    }
    
    AABB bounding_box() const {
        return bbox;
    }
    
    static void get_uv(const Vec3& p, double& u, double& v) {
        auto phi = std::atan2(-p.Z(), p.X()) + pi;
        auto theta = std::acos(-p.Y());
        u = phi / (2*pi);
        v = theta / pi;
    }
    
#if HIT_IMPL == 1
    
    bool hit(const Ray& ray, const Interval& limits, Hit& hit) const {
        // manual inline is slower, why?
        return intersect(ray, limits, hit);

    }
    
    // Geometric solution
    inline bool intersect(const Ray &ray, Interval limits, Hit& hit) const
    {
        Vec3 center_dt = center + velocity * ray.time();
        double t0, t1; // solutions for t if the ray intersects
        Vec3 L = center_dt - ray.origin();
        double tca = dot(L, ray.dir());
        // if (tca < 0) return false;
        double d2 = dot(L, L) - tca * tca;
        if (d2 > r*r) return false;
        double thc = sqrt(r*r - d2);
        t0 = tca - thc;
        t1 = tca + thc;
        double d = t0;
        if (!limits.surrounds(t0)) {
            d = t1;
            if (!limits.surrounds(d))
                return false;
        }
        hit.d = d;
        hit.p = ray.at(hit.d);
        Vec3 normal = (hit.p - center_dt) / r;
        hit.set_normal(ray, normal);
        get_uv(hit.n, hit.u, hit.v);
        hit.material = material;
        return true;
    }
#endif
    
#if HIT_IMPL == 0
    // https://github.com/RayTracing/raytracing.github.io/
    bool hit(const Ray& ray, Interval limits, Hit& hit) {
        
        Vec3 center_dt = center + velocity * ray.time();
        Vec3 OC = center_dt - ray.origin();
        double a = ray.dir().len_sq();
        double h = dot(ray.dir(), OC);
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
        hit.n = (hit.p - center_dt) / r;
        hit.is_front = dot(ray.dir(), hit.n) < 0;
        hit.n = hit.is_front ? hit.n : -1 * hit.n;
        hit.material = material;
        
        return true;
    }
    
#endif
    
    
#if HIT_IMPL == 2
    // Analytic solution
    inline bool hit(const Ray &ray, Interval limits, Hit& hit) const
    {
        Vec3 center_dt = center + velocity * ray.time();
        double t0, t1; // solutions for t if the ray intersects
        Vec3 L = ray.origin() - center_dt;
        double a = dot(ray.dir(), ray.dir());
        double b = 2 * dot(ray.dir(), L);
        double c = dot(L,L) - r*r;
        if (!solveQuadratic(a, b, c, t0, t1)) return false;
        double d = t0;
        if (!limits.surrounds(t0)) {
            d = t1;
            if (!limits.surrounds(d))
                return false;
        }
        hit.d = d;
        hit.p = ray.at(hit.d);
        hit.n = (hit.p - center_dt) / r;
        hit.is_front = dot(ray.dir(), hit.n) < 0;
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
    
#endif
    
};


#endif /* geometry_h */
