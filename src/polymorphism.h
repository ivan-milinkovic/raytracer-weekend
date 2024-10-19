#ifndef polymorphism_h
#define polymorphism_h
class Hittable;
class Ray;
class Interval;
class Hit;
class AABB;

bool hit_impl(const Hittable* object, const Ray& ray, const Interval& interval, Hit& hit);
AABB get_bbox_impl(const Hittable* object);

#endif /* polymorphism_h */
