/*
#include "hittable.h"
#include "bvh.h"
#include "polymorphism.h"

bool hit_impl(const Hittable* object, const Ray& ray, const Interval& interval, Hit& hit) {
    switch (object->type)
    {
        case HittableType_Sphere: {
            const Sphere* sphere = static_cast<const Sphere*>(object);
            return sphere->hit(ray, interval, hit);
            break;
        }
            
        case HittableType_BVH_Node: {
            const BVH_Node* bvh_node = static_cast<const BVH_Node*>(object);
            return bvh_node->hit(ray, interval, hit);
            break;
        }
    }
    return false;
}

AABB get_bbox_impl(const Hittable* object) {
    switch (object->type)
    {
        case HittableType_Sphere: {
            auto sphere = static_cast<const Sphere*>(object);
            return sphere->bbox;
            break;
        }
        case HittableType_BVH_Node: {
            auto box = static_cast<const BVH_Node*>(object);
            return box->bbox;
            break;
        }
    }
    return AABB();
}
*/
