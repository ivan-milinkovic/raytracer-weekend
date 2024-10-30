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
    Material* material; // raw pointer up to 30% faster than shared_ptr (many assignments)
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
    HittableType_List,
    HittableType_Translate,
    HittableType_RotateY,
    HittableType_Sphere,
    HittableType_Quad,
    HittableType_ConstantMedium,
} HittableType;


class Hittable {
public:
    HittableType type;
    
    Hittable(HittableType type): type(type) { }
    
    inline bool hit(const Ray& ray, const Interval& limits, Hit& hit);
    inline AABB bounding_box();
    
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

    
    bool hit(const Ray& r, const Interval& limits, Hit& hit) const {
        // Instead of objects, move the ray
        Ray offset_r(r.origin() - offset, r.dir(), r.time());
        if (!object->hit(offset_r, limits, hit))
            return false;
        hit.p += offset;
        return true;
    }
    
    AABB bounding_box() const { return bbox; }

  private:
    shared_ptr<Hittable> object;
    Vec3 offset;
    AABB bbox;
};


class RotateY : public Hittable {
  public:
    
    RotateY(shared_ptr<Hittable> object, double angle) : Hittable(HittableType_RotateY), object(object) {
        auto radians = rad_from_deg(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        Vec3 min( infinity,  infinity,  infinity);
        Vec3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.xi.max + (1-i)*bbox.xi.min;
                    auto y = j*bbox.yi.max + (1-j)*bbox.yi.min;
                    auto z = k*bbox.zi.max + (1-k)*bbox.zi.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min.set(c, std::fmin(min[c], tester[c]));
                        max.set(c, std::fmax(max[c], tester[c]));
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }

    bool hit(const Ray& r, const Interval& limits, Hit& hit) const {

        // Transform the ray from world space to object space.

        auto origin = Vec3(
            (cos_theta * r.origin().X()) - (sin_theta * r.origin().Z()),
            r.origin().Y(),
            (sin_theta * r.origin().X()) + (cos_theta * r.origin().Z())
        );

        auto direction = Vec3(
            (cos_theta * r.dir().X()) - (sin_theta * r.dir().Z()),
            r.dir().Y(),
            (sin_theta * r.dir().X()) + (cos_theta * r.dir().Z())
        );

        Ray rotated_r(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where).

        if (!object->hit(rotated_r, limits, hit))
            return false;

        // Transform the intersection from object space back to world space.

        hit.p = Vec3(
            (cos_theta * hit.p.X()) + (sin_theta * hit.p.Z()),
            hit.p.Y(),
            (-sin_theta * hit.p.X()) + (cos_theta * hit.p.Z())
        );

        hit.n = Vec3(
            (cos_theta * hit.n.X()) + (sin_theta * hit.n.Z()),
            hit.n.Y(),
            (-sin_theta * hit.n.X()) + (cos_theta * hit.n.Z())
        );

        return true;
    }
    
    AABB bounding_box() const { return bbox; }
    
private:
    shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    AABB bbox;
};

#endif /* hittable_h */
