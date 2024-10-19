#ifndef hittable_h
#define hittable_h

#include "vec3.h"
class Material;
class AABB;

class Hit {
public:
    Vec3 p;
    Vec3 n;
    double d; // distance from ray origin
    
    Material* material = nullptr;
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
    
    
    // avoiding v-tables, not faster
    /*
    inline bool hit(const Ray& ray, const Interval& interval, Hit& hit) const {
        return hit_impl(this, ray, interval, hit);
    }
    
    inline const AABB* bounding_box() const {
        return get_bbox_impl(this);
    }
     */
};


#endif /* hittable_h */
