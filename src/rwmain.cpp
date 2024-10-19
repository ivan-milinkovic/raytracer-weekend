#include <memory>
using std::make_shared;
#include "image.h"
#include "camera.h"
#include "geometry.h"
#include "scene.h"

// viewport - A projection plane in 3D space. In world space, not view space:
//            because objects are not projected, not transformed to view space.
//            Instead, the camera is moved and it generates rays from it's own transform.
// screen - an image on monitor (screen space)


class State {
public:
    std::unique_ptr<Image> image;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Camera> camera;
    
    double screen_aspect;
    int screen_W;
    int screen_H;
};

State state = State();
Image* getImage() { return state.image.get(); }


void init();
void init_image();
void init_scene_3_balls();
void init_scene_bouncing_balls();
void render();


void rwmain()
{
    init();
    
    auto t0 = std::chrono::high_resolution_clock::now();
    
    render();
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << dt << "ms\n";
}

void init() {
    init_image();
    switch(2) {
        case 1: init_scene_3_balls(); break;
        case 2: init_scene_bouncing_balls(); break;
    }
}

void init_image() {
    state.screen_aspect = 16.0 / 9.0;
    state.screen_W = 600;
    state.screen_H = (int) (state.screen_W / state.screen_aspect);
    state.image = std::make_unique<Image>(state.screen_W, state.screen_H);
    state.scene = std::make_unique<Scene>();
}

inline void render() {
    state.camera->render(*state.scene, *state.image);
}

void init_scene_3_balls() {
    
    auto material_ground = make_shared<LambertianMaterial> ( Vec3(0.5, 0.5, 0.5) );
    auto material_center = make_shared<LambertianMaterial> ( Vec3(0.1, 0.2, 0.5) );
    auto material_left   = make_shared<DielectricMaterial> ( 1.5 );
    auto material_bubble = make_shared<DielectricMaterial> ( 1.0 / 1.5 );
    auto material_right  = make_shared<MetalMaterial>      ( Vec3(0.8, 0.6, 0.2), 0.16 );
    
    auto center = make_shared<Sphere>( Vec3( 0, 0, 3), 1, material_center);
    auto left   = make_shared<Sphere>( Vec3(-2, 0, 3), 1, material_left);
    auto bubble = make_shared<Sphere>( Vec3(-2, 0, 3), 0.75, material_bubble);
    auto right  = make_shared<Sphere>( Vec3( 2, 0, 3), 1, material_right);
    auto ground = make_shared<Sphere>( Vec3( 0, -101, 3), 100, material_ground);
    
    state.scene->objects.push_back( center );
    state.scene->objects.push_back( left );
    state.scene->objects.push_back( bubble );
    state.scene->objects.push_back( right );
    state.scene->objects.push_back( ground );
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    
    state.camera->vfov_deg = 70;
    state.camera->focus_dist = 1;
    state.camera->defocus_angle = 0;
    state.camera->setup();
    state.camera->look_from_at({ 0, 0, 0 }, { 0, 0, 1 });
    
//    state.camera->vfov_deg = 40;
//    state.camera->focus_dist = 4.7;
//    state.camera->defocus_angle = 4;
//    state.camera->setup();
//    state.camera->look_from_at({ -4.5,0,0.5 }, { 0,0,2 });
}

void init_scene_bouncing_balls() {
    
    auto material_ground = make_shared<LambertianMaterial>( Vec3(0.5, 0.5, 0.5) );
    auto material1 = make_shared<DielectricMaterial> ( 1.5 );
    auto material2 = make_shared<LambertianMaterial> ( Vec3(0.4, 0.2, 0.1) );
    auto material3 = make_shared<MetalMaterial> ( Vec3(0.7, 0.6, 0.5), 0.0 );

    state.scene->objects.push_back( make_shared<Sphere>( Vec3(0, -1000, 3), 1000, material_ground) );
    state.scene->objects.push_back( make_shared<Sphere>( Vec3(0, 1, 0), 1, material1) );
    state.scene->objects.push_back( make_shared<Sphere>( Vec3(-4, 1, 0), 1, material2) );
    state.scene->objects.push_back( make_shared<Sphere>( Vec3(4, 1, 0), 1, material3) );
    
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            Vec3 center( a + 0.9 * rw_random(), 0.2, b + 0.9 * rw_random() );
            if ((center - Vec3(4, 0.2, 0)).len() > 0.9)
            {
                auto mat_dist = rw_random();
                if (mat_dist < 0.8)
                {
                    Vec3 color = Vec3::random() * Vec3::random();
                    auto mat = make_shared<LambertianMaterial>( color );
                    Vec3 center2 = center + Vec3(0, 0.3, 0);
                    state.scene->objects.push_back( make_shared<Sphere>( center, center2, 0.2, mat) );
                }
                else if (mat_dist < 0.95)
                {
                    Vec3 color = Vec3::random(0.5, 1);
                    double fuzz = rw_random(0, 0.5);
                    auto mat = make_shared<MetalMaterial>( color, fuzz );
                    state.scene->objects.push_back( make_shared<Sphere>( center, 0.2, mat) );
                }
                else
                {
                    auto mat = make_shared<DielectricMaterial> ( 1.5 );
                    state.scene->objects.push_back( make_shared<Sphere>( center, 0.2, mat) );
                }
            }
        }
    }
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 20;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.6;
    state.camera->samples_per_pixel = 10;
    state.camera->max_bounces = 10;
    state.camera->setup();
    state.camera->look_from_at({ 13,2,3 }, { 0,0,0 });
}
