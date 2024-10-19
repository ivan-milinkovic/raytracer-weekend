#ifndef Scene_h
#define Scene_h

#include <memory>
using std::shared_ptr;
using std::vector;
#include "interval.h"
#include "geometry.h"
#include "bvh.h"
#include "hittable.h"
#include "material.h"

class Scene {
public:
    vector<shared_ptr<Hittable>> objects;
    vector<shared_ptr<Material>> materials;
    BVH_Node* bvh_root; // 2x speed up
    
    bool hit(const Ray& ray, const Interval& limits, Hit& hit) const {
        return bvh_root->hit(ray, limits, hit);
    }
    
    void make_bvh() {
        bvh_root = new BVH_Node(objects);
    }
    
};

#endif /* Scene_h */
