#include "image.h"
#include "ray.h"
#include "geometry.h"
#include "scene.h"
#import "camera.h"

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

// min and max distances for ray-geometry intersections
#define ray_hit_min 0.2
#define ray_hit_max 100

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
    SceneObject so(SceneObjectType_Sphere, new Sphere({0,0,3}, 1));
    state.scene->objects.push_back( so );
    
//    SceneObject so2(SceneObjectType_Sphere, new Sphere({-2,0,5}, 1));
//    state.scene->objects.push_back( so2 );
//    
//    SceneObject so3(SceneObjectType_Sphere, new Sphere({2,0,5}, 1));
//    state.scene->objects.push_back( so3 );
}


Vec3 ray_color(const Ray& ray);

void rwmain()
{
    init_image();
    init_camera();
    init_scene();
    
    Image& image = *state.image;
    Camera& camera = *state.camera;

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
            pixel = ray_color(ray);
        }
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << dt << "ms\n";
}

Vec3 ray_color(const Ray& ray)
{
    Vec3 color;
    Hit hit;
    if (state.scene->hit(ray, ray_hit_min, ray_hit_max, hit))
    {
        color = 0.5 * Vec3AddScalar(hit.n, 1.0);
        
        // print_csv(hit.n);
        // print_csv(hit.p, hit.p + hit.n);
    }
    else // background
    {
        double f = (ray.Dir().Y() + 1.0) * 0.5;
        color = ((1-f) * Vec3(1, 1, 1)) + (f * Vec3(0, 0.3, 0.8));
    }
    return color;
}
