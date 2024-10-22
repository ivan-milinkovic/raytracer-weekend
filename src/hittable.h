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
    HittableType_Translate,
    HittableType_Sphere,
    HittableType_Quad,
} HittableType;


class Hittable {
public:
    HittableType type;
    
    Hittable(HittableType type): type(type) { }
    
    virtual inline bool hit(const Ray& ray, const Interval& limits, Hit& hit) const = 0;
    virtual inline AABB bounding_box() const = 0;
    
    // Tried avoiding v-tables, but wasn't much faster, time is spent elsewhere.
    // Also, hard to do static dispatch at this point,
    // because logic is in headers, and cyclic dependencies cause errors
};


class Translate : public Hittable {
  public:
    Translate(shared_ptr<Hittable> object, const Vec3& offset)
    : Hittable(HittableType_Translate), object(object), offset(offset)
        {
            bbox = object->bounding_box() + offset;
        }

    
    bool hit(const Ray& r, const Interval& limits, Hit& hit) const override {
        // Instead of objects, move the ray
        Ray offset_r(r.origin() - offset, r.dir(), r.time());
        if (!object->hit(offset_r, limits, hit))
            return false;
        hit.p += offset;
        return true;
    }
    
    AABB bounding_box() const override { return bbox; }

  private:
    shared_ptr<Hittable> object;
    Vec3 offset;
    AABB bbox;
};

#endif /* hittable_h */
