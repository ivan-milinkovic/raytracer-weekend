//
//  Scene.h
//  RaytracerWeekendConsole
//
//  Created by Ivan Milinkovic on 12.10.24..
//

#ifndef Scene_h
#define Scene_h

#include "interval.h"
#include "geometry.h"

typedef enum {
    SceneObjectType_Sphere = 1
} SceneObjectType;


class SceneObject {
public:
    int id;
    SceneObjectType type;
    void* object;
};


class Scene {
public:
    std::vector<SceneObject> objects;
    
    bool hit(const Ray& ray, const Interval& limits, Hit& hit) const {
        double closest_so_far = limits.max;
        Hit current_hit;
        bool has_hit = false;
        
        for (const SceneObject& sceneObject : objects) {
            switch (sceneObject.type)
            {
                case SceneObjectType_Sphere: {
                    Sphere* sphere = static_cast<Sphere*>(sceneObject.object);
                    if (sphere->hit(ray, Interval(limits.min, closest_so_far), current_hit)) {
                        hit = current_hit;
                        closest_so_far = current_hit.d;
                        has_hit = true;
                    }
                    break;
                }
            }
        }
        return has_hit;
    }
    
    ~Scene() {
        for ( SceneObject& sceneObject : objects) {
            switch (sceneObject.type)
            {
                case SceneObjectType_Sphere:
                    Sphere* sphere = static_cast<Sphere*>(sceneObject.object);
                    delete sphere;
                    break;
            }
        }
    }
};

#endif /* Scene_h */
