#include "image.h"
#import "camera.h"
#include "geometry.h"
#include "scene.h"

// viewport - A projection plane in 3D space. In world space, not view space:
//            because objects are not projected, not transformed to view space.
//            Instead, the camera is moved and it generates rays from it's own transform.
// screen - an image on monitor (screen space)


class State {
public:
    Image* image;
    Scene* scene;
    Camera* camera;
    
    double screen_aspect;
    int screen_W;
    int screen_H;
    
    ~State() {
        delete image;
        delete scene;
        delete camera;
    }
    
};

State state = State();
Image* getImage() { return state.image; }

void init_image() {
    state.screen_aspect = 16.0 / 9.0;
    state.screen_W = 600;
    state.screen_H = (int) (state.screen_W / state.screen_aspect);
    state.image = new Image(state.screen_W, state.screen_H);
    state.scene = new Scene();
}

void init_camera() {
    state.camera = new Camera(state.screen_W, state.screen_H);
}

void init_scene() {
    SceneObject so(SceneObjectType_Sphere, new Sphere( { 0, 0, 3 }, 1));
    state.scene->objects.push_back( so );
    
//    SceneObject so2(SceneObjectType_Sphere, new Sphere({-2,0,5}, 1));
//    state.scene->objects.push_back( so2 );
//    
//    SceneObject so3(SceneObjectType_Sphere, new Sphere({2,0,5}, 1));
//    state.scene->objects.push_back( so3 );
    
    SceneObject so4(SceneObjectType_Sphere, new Sphere( { 0, -101, 3 }, 100));
    state.scene->objects.push_back( so4 );
}

void init() {
    init_image();
    init_camera();
    init_scene();
}

inline void render() {
    state.camera->render(*state.scene, *state.image);
}

void rwmain()
{
    init();
    
    auto t0 = std::chrono::high_resolution_clock::now();
    
    render();
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << dt << "ms\n";
}
