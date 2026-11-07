#ifndef bvh_h
#define bvh_h

#include <algorithm>
#include "aabb.h"
#include "sphere.h"
#include "hittable_list.h"


// Bounding Volume Hierarchy
class BVH_Node: public Hittable {
public:
    AABB bbox;
    Hittable* left;
    Hittable* right;
    Arena& arena;
    
    BVH_Node(Arena& arena) : Hittable(HittableType_BVH_Node), arena(arena) {}
    
    BVH_Node(std::vector<std::shared_ptr<Hittable>>& objects, Arena& arena)
    : BVH_Node(objects, 0, objects.size(), arena) { }
    
    BVH_Node(HittableList list, Arena& arena) : BVH_Node(list.objects, 0, list.objects.size(), arena) { }
    
    BVH_Node(std::vector<std::shared_ptr<Hittable>>& objects,
             size_t start,
             size_t end,
             Arena& arena
    ): Hittable(HittableType_BVH_Node), arena(arena) {
        
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
            left = right = objects[start].get();
        }
        else if (subset_len == 2) {
            left = objects[start].get();
            right = objects[start + 1].get();
        }
        else {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);
            auto mid = start + subset_len/2;
            left = alloc_node(objects, start, mid, arena);
            right = alloc_node(objects, mid, end, arena);
        }
    }
    
    BVH_Node* alloc_node(std::vector<std::shared_ptr<Hittable>>& objects,
                        size_t start,
                        size_t end,
                        Arena& arena) {
        auto lptr = arena.allocate<BVH_Node>();
        return new (lptr) BVH_Node(objects, start, end, arena);
    }
    
    AABB bounding_box() const {
        return bbox;
    }
    
    bool hit(const Ray& ray, const Interval& interval, Hit& hit) const {
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
