#ifndef Scene_h
#define Scene_h

#include <memory>
#include "interval.h"
#include "geometry.h"
#include "bvh.h"
#include "hittable.h"
#include "material.h"
#include "free_impl.h"

class Scene {
public:
    std::vector<Hittable*> objects;
    std::vector<Material*> materials;
    BVH_Node* bvh_root; // 2x speed up
    
    bool hit(const Ray& ray, const Interval& limits, Hit& hit) const {
        return bvh_root->hit(ray, limits, hit);
    }
    
    void make_bvh() {
        std::vector<std::shared_ptr<Hittable>> hittables;
        for (Hittable* h : objects) {
            hittables.push_back( std::shared_ptr<Hittable>(h) );
        }
        bvh_root = new BVH_Node(hittables);
    }
    
    ~Scene() {
        delete_hittables(objects);
        delete_materials(materials);
//        delete bvh_root;
    }
};

#endif /* Scene_h */
