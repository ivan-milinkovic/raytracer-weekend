#ifndef aabb_h
#define aabb_h

#include "vec3.h"
#include "ray.h"
#include "interval.h"


class AABB {
public:
    Interval xi, yi, zi;
    
    AABB() { }
    
    AABB(const Interval& xi, const Interval& yi, const Interval& zi)
    : xi(xi), yi(yi), zi(zi) {
        apply_minimums();
    }
    
    AABB(const Vec3& p1, const Vec3& p2)
    {
        xi.min = std::min(p1.X(), p2.X());
        xi.max = std::max(p1.X(), p2.X());
        
        yi.min = std::min(p1.Y(), p2.Y());
        yi.max = std::max(p1.Y(), p2.Y());
        
        zi.min = std::min(p1.Z(), p2.Z());
        zi.max = std::max(p1.Z(), p2.Z());
    }
    
    AABB(const AABB& box0, const AABB& box1) {
        xi = Interval(box0.xi, box1.xi);
        yi = Interval(box0.yi, box1.yi);
        zi = Interval(box0.zi, box1.zi);
    }
    
    // Quads don't have thikness, so one bbox dimension will be 0 - apply min size
    void apply_minimums() {
        double delta = 0.00025;
        if (xi.size() < delta) xi = xi.expanded(delta);
        if (yi.size() < delta) yi = yi.expanded(delta);
        if (zi.size() < delta) zi = zi.expanded(delta);
    }
    
    const Interval& axis_interval(int axis) const {
        if (axis == 0) return xi;
        if (axis == 1) return yi;
        return zi;
    }
    
    bool hit(const Ray& ray, Interval ray_dt) const {
        const Vec3& ro = ray.origin();
        const Vec3& rd = ray.dir();
        
        for (int axis=0; axis<3; axis++) {
            const Interval& interval = axis_interval(axis);
            const double r_axis_inv = 1.0 / rd[axis];
            
            double t0 = ( interval.min - ro[axis] ) * r_axis_inv;
            double t1 = ( interval.max - ro[axis] ) * r_axis_inv;
            
            if (t0 > t1) std::swap(t0, t1);
            if (t0 > ray_dt.min) ray_dt.min = t0;
            if (t0 < ray_dt.max) ray_dt.max = t1;
            
            if (ray_dt.max <= ray_dt.min) return false;
        }
        return true;
    }
    
    int longest_axis() const {
        if (xi.size() > yi.size())
            return xi.size() > zi.size() ? 0 : 2;
        else
            return yi.size() > zi.size() ? 1 : 2;
        }
    
    static const AABB empty, universe;
};

const AABB AABB::empty    = AABB(Interval::empty,    Interval::empty,    Interval::empty);
const AABB AABB::universe = AABB(Interval::universe, Interval::universe, Interval::universe);


#endif /* aabb_h */
