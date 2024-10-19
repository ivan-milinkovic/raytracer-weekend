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
    Image* image;
    Scene* scene;
    Camera* camera;
    std::vector<Material*> materials;
    
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


void init();
void init_image();
void init_scene_1();
void init_scene_2();
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
//    init_scene_1();
    init_scene_2();
}

void init_image() {
    state.screen_aspect = 16.0 / 9.0;
    state.screen_W = 600;
    state.screen_H = (int) (state.screen_W / state.screen_aspect);
    state.image = new Image(state.screen_W, state.screen_H);
    state.scene = new Scene();
}

inline void render() {
    state.camera->render(*state.scene, *state.image);
}

void init_scene_1() {
    
    auto material_ground = new LambertianMaterial ( { 0.5, 0.5, 0.5 } );
    auto material_center = new LambertianMaterial ( { 0.1, 0.2, 0.5 } );
    auto material_left   = new DielectricMaterial ( 1.5 );
    auto material_bubble = new DielectricMaterial ( 1.0 / 1.5 );
    auto material_right  = new MetalMaterial      ( { 0.8, 0.6, 0.2 }, 0.16 );
    
    SceneObject center  (1, SceneObjectType_Sphere, new Sphere( { 0, 0, 3 }, 1, material_center));
    SceneObject left    (2, SceneObjectType_Sphere, new Sphere( {-2, 0, 3 }, 1, material_left));
    SceneObject bubble  (3, SceneObjectType_Sphere, new Sphere( {-2, 0, 3 }, 0.75, material_bubble));
    SceneObject right   (4, SceneObjectType_Sphere, new Sphere( { 2, 0, 3 }, 1, material_right));
    SceneObject ground  (5, SceneObjectType_Sphere, new Sphere( { 0, -101, 3 }, 100, material_ground));
    
    state.materials.push_back(material_ground);
    state.materials.push_back(material_center);
    state.materials.push_back(material_left);
    state.materials.push_back(material_bubble);
    state.materials.push_back(material_right);
    
    state.scene->objects.push_back( center );
    state.scene->objects.push_back( left );
    state.scene->objects.push_back( bubble );
    state.scene->objects.push_back( right );
    state.scene->objects.push_back( ground );
    
    state.camera = new Camera(state.screen_W, state.screen_H);
    
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

void init_scene_2() {
    
    auto material_ground = new LambertianMaterial ( { 0.5, 0.5, 0.5 } );
    state.scene->objects.push_back(
        SceneObject(0, SceneObjectType_Sphere,
                    new Sphere( { 0, -1000, 3 }, 1000, material_ground))
    );
    
    auto material1 = new DielectricMaterial ( 1.5 );
    state.scene->objects.push_back(
        SceneObject(0, SceneObjectType_Sphere,
                    new Sphere( { 0, 1, 0 }, 1, material1))
    );
    
    auto material2 = new LambertianMaterial ( { 0.4, 0.2, 0.1 } );
    state.scene->objects.push_back(
        SceneObject(0, SceneObjectType_Sphere,
                    new Sphere( { -4, 1, 0 }, 1, material2))
    );
    
    auto material3 = new MetalMaterial ( { 0.7, 0.6, 0.5 }, 0.0 );
    state.scene->objects.push_back(
        SceneObject(0, SceneObjectType_Sphere,
                    new Sphere( { 4, 1, 0 }, 1, material3))
    );
    
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            Vec3 center( a + 0.9 * rw_random(), 0.2, b + 0.9 * rw_random() );
            if ((center - Vec3(4, 0.2, 0)).len() > 0.9)
            {
                auto mat_dist = rw_random();
                Material* mat;
                if (mat_dist < 0.8)
                {
                    Vec3 color = Vec3::random() * Vec3::random();
                    mat = new LambertianMaterial ( color );
                    Vec3 velocity = { 0, 0.2, 0 };
                    state.scene->objects.push_back(
                        SceneObject(0, SceneObjectType_Sphere,
                                    new Sphere( center, velocity, 0.2, mat))
                    );
                }
                else if (mat_dist < 0.95)
                {
                    Vec3 color = Vec3::random(0.5, 1);
                    double fuzz = rw_random(0, 0.5);
                    mat = new MetalMaterial ( color, fuzz );
                    state.scene->objects.push_back(
                        SceneObject(0, SceneObjectType_Sphere,
                                    new Sphere( center, 0.2, mat))
                    );
                }
                else
                {
                    mat = new DielectricMaterial ( 1.5 );
                    state.scene->objects.push_back(
                        SceneObject(0, SceneObjectType_Sphere,
                                    new Sphere( center, 0.2, mat))
                    );
                }
            }
        }
    }
    
    state.camera = new Camera(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 20;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.6;
    state.camera->samples_per_pixel = 10;
    state.camera->max_bounces = 10;
    state.camera->setup();
    state.camera->look_from_at({ 13,2,3 }, { 0,0,0 });
}
