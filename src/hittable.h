#ifndef hittable_h
#define hittable_h

#include "vec3.h"
class Material;
class Ray;
class Interval;
class AABB;

class Hit {
public:
    Vec3 p;
    Vec3 n;
    double d; // distance from ray origin
    std::shared_ptr<Material> material;
    double u;
    double v;
    bool is_front;
    
    inline void set_normal(const Ray& r, const Vec3& outward_normal) {
        is_front = dot(r.dir(), outward_normal) < 0;
        n = is_front ? outward_normal : -outward_normal;
    }
};

typedef enum {
    HittableType_BVH_Node = 1,
    HittableType_Sphere,
    HittableType_Quad,
} HittableType;


class Hittable {
public:
    HittableType type;
    
    Hittable(HittableType type): type(type) { }
    
    virtual inline bool hit(const Ray& ray, const Interval& interval, Hit& hit) const = 0;
    virtual inline AABB bounding_box() const = 0;
    
    // Tried avoiding v-tables, but wasn't much faster, time is spent elsewhere.
    // Also, hard to do static dispatch at this point,
    // because logic is in headers, and cyclic dependencies cause errors
};

#endif /* hittable_h */
