#ifndef Scene_h
#define Scene_h

#include <memory>
using std::shared_ptr;
using std::vector;
#include "bvh.h"
#include "hittable.h"

class Scene {
    
public:
    vector<shared_ptr<Hittable>> objects;
    BVH_Node* bvh_root; // 2x speed up, compared to iterating objects array
    Arena* arena;
    
    void add(shared_ptr<Hittable> obj) {
        objects.push_back(obj);
    }
    
    void add(vector<shared_ptr<Hittable>> objs) {
        objects.insert(objects.end(), objs.begin(), objs.end());
    }
    
    bool hit(const Ray& ray, const Interval& limits, Hit& hit) const {
        return bvh_root->hit(ray, limits, hit);
    }
    
    void make_bvh() {
//        bvh_root = new BVH_Node(objects);
        auto ptr = arena->allocate<BVH_Node>();
        bvh_root = new (ptr) BVH_Node(objects);
    }
    
};

#endif /* Scene_h */
