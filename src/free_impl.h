//
//  free_impl.h
//  RaytracerWeekendConsole
//
//  Created by Ivan Milinkovic on 19.10.24..
//

#ifndef free_impl_h
#define free_impl_h

#include "geometry.h"
#include "bvh.h"
#include "material.h"


void delete_hittables(std::vector<Hittable*> hittables) {
    for ( const Hittable* object : hittables) {
        switch (object->type)
        {
            case HittableType_Sphere: {
                delete static_cast<const Sphere*>(object);
                break;
            }
            case HittableType_BVH_Node: {
                delete static_cast<const BVH_Node*>(object);
                break;
            }
        }
    }
}

void delete_materials(std::vector<Material*> materials) {
    for ( const Material* material : materials) {
        switch (material->type) {
            case MaterialType_Lambertian: {
                delete static_cast<const LambertianMaterial*>(material);
                break;
            }
            case MaterialType_Metal: {
                delete static_cast<const MetalMaterial*>(material);
                break;
            }
            case MaterialType_Dielectric: {
                delete static_cast<const DielectricMaterial*>(material);
                break;
            }
        }
    }
}


#endif /* free_impl_h */
