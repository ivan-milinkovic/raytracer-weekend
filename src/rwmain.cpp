#include "Image.h"
#include "Ray.h"
#include "geometry.h"
#include "Scene.h"
#import "Camera.h"

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

void rwmain()
{
    init_image();
    init_camera();
    
    Image& image = *state.image;
    Camera& camera = *state.camera;
    
    // world
    Sphere sphere({0,0,4}, 1);
    
    // render
    
    auto t0 = std::chrono::high_resolution_clock::now();
    
    for (int r = 0; r < image.H(); r++)
    {
        for (int c = 0; c < image.W(); c++)
        {
            Vec3 viewport_point;
            Ray ray = camera.make_ray(c, r, viewport_point);
            
            int i = r * image.W() + c;
            Vec3& pixel = image[i];
            
            Hit hit;
            if (sphere.ray_hit(ray, 0.2, 100, hit))
            {
                pixel = 0.5 * Vec3AddScalar(hit.n, 1.0);
            }
            else {
                double f = (ray.Dir().Y() + 1.0) * 0.5;
                pixel = ((1-f) * Vec3(1, 1, 1)) + (f * Vec3(0, 0.3, 0.8));
            }
        }
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << dt << "ms\n";
}
