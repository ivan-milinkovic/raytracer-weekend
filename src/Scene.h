//
//  Scene.h
//  RaytracerWeekendConsole
//
//  Created by Ivan Milinkovic on 12.10.24..
//

#ifndef Scene_h
#define Scene_h

typedef enum {
    SceneObjectType_Sphere = 1
} SceneObjectType;

class SceneObject {
    SceneObjectType type;
    void* object;
};

class Scene {
    std::vector<SceneObject> objects;
};

#endif /* Scene_h */
