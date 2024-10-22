#ifndef bvh_h
#define bvh_h

#include <algorithm>
#include "aabb.h"
#include "sphere.h"

// Bounding Volume Hierarchy
class BVH_Node: public Hittable {
public:
    AABB bbox;
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    
    BVH_Node() : Hittable(HittableType_BVH_Node) {}
    
    BVH_Node(std::vector<std::shared_ptr<Hittable>>& objects)
    : BVH_Node(objects, 0, objects.size()) {
        
    }
    
    BVH_Node(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
    : Hittable(HittableType_BVH_Node) {
        
        // Bounding box of the given subset of objects
        bbox = AABB::empty;
        for (size_t i=start; i < end; i++)
            bbox = AABB(bbox, objects[i]->bounding_box());

        int axis = bbox.longest_axis(); // 50ms speedup for the bouncing spheres sccene (470ms -> 420ms)
        
        auto comparator = (axis == 0) ? box_x_compare
                          : (axis == 1) ? box_y_compare
                          : box_z_compare;
        
        auto subset_len = end - start;
        if (subset_len == 1) {
            left = right = objects[start];
        }
        else if (subset_len == 2) {
            left = objects[start];
            right = objects[start + 1];
        }
        else {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);
            auto mid = start + subset_len/2;
            left = std::make_shared<BVH_Node>(objects, start, mid);
            right = std::make_shared<BVH_Node>(objects, mid, end);
        }
    }
    
    AABB bounding_box() const override {
        return bbox;
    }
    
    bool hit(const Ray& ray, const Interval& interval, Hit& hit) const override {
        if ( !bbox.hit(ray, interval) )
            return false;
        bool in_left = left->hit(ray, interval, hit);
        bool in_right = right->hit(ray, Interval(interval.min, in_left ? hit.d : interval.max), hit);
        return in_left || in_right;
    }
    
    static bool box_compare(const std::shared_ptr<Hittable> a,
                            const std::shared_ptr<Hittable> b,
                            int axis_index )
    {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }
    
    static bool box_x_compare (const std::shared_ptr<Hittable> a,
                               const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 0);
    }
    
    static bool box_y_compare (const std::shared_ptr<Hittable> a,
                               const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 1);
    }
    
    static bool box_z_compare (const std::shared_ptr<Hittable> a,
                               const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 2);
    }
};

#endif /* bvh_h */
