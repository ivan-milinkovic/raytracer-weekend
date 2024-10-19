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
};

typedef enum {
    HittableType_Sphere = 1,
    HittableType_BVH_Node = 2
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
