#include <memory>
using std::make_shared;
#include "image.h"
#include "camera.h"
#include "sphere.h"
#include "quad.h"
#include "scene.h"
#include "rwimage.h"
#include "hittable_list.h"

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
void init_scene_earth();
void init_scene_perlin_spheres();
void init_scene_quads();
void init_scene_light();
void init_scene_cornell_box();
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
    switch(1) {
        case 1: init_scene_bouncing_balls(); break;
        case 2: init_scene_3_balls(); break;
        case 3: init_scene_earth(); break;
        case 4: init_scene_perlin_spheres(); break;
        case 5: init_scene_quads(); break;
        case 6: init_scene_light(); break;
        case 7: init_scene_cornell_box(); break;
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
    state.camera->background = Vec3(0.70, 0.80, 1.00);
    
//    state.camera->vfov_deg = 40;
//    state.camera->focus_dist = 4.7;
//    state.camera->defocus_angle = 4;
//    state.camera->setup();
//    state.camera->look_from_at({ -4.5,0,0.5 }, { 0,0,2 });
}

void init_scene_bouncing_balls() {
    
    // auto material_ground = make_shared<LambertianMaterial>( Vec3(0.5, 0.5, 0.5) );
    auto texture_checker = make_shared<CheckerTexture>(0.32, Vec3(0.05, 0.05, .4), Vec3(.9, .9, .9));
    auto material_ground = make_shared<LambertianMaterial> ( texture_checker );
    
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
                    Vec3 center2 = center + Vec3(0, 0.2, 0);
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
    state.camera->background = Vec3(0.70, 0.80, 1.00);
    state.camera->setup();
    state.camera->look_from_at({ 13,2,3 }, { 0,0,0 });
}

void init_scene_earth()
{
    auto file_path = root_dir() / "res" / "tex.png";
    auto earth_texture = make_shared<ImageTexture>(file_path.c_str());
    state.scene->objects.push_back(
        make_shared<Sphere>(Vec3(0, 0, 0), 2, make_shared<LambertianMaterial>(earth_texture))
    );
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 20;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.0;
    state.camera->samples_per_pixel = 10;
    state.camera->max_bounces = 10;
    state.camera->background = Vec3(0.70, 0.80, 1.00);
    state.camera->setup();
    state.camera->look_from_at({ 0,0,-12 }, { 0,0,0 });
}

void init_scene_perlin_spheres()
{
    auto perlin_texture = make_shared<PerlinTexture>(4);
    state.scene->objects.push_back(
        make_shared<Sphere>(Vec3(0,-1000,0), 1000, make_shared<LambertianMaterial>(perlin_texture))
    );
    state.scene->objects.push_back(
        make_shared<Sphere>(Vec3(0,2,0), 2, make_shared<LambertianMaterial>(perlin_texture))
    );
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 20;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.0;
    state.camera->samples_per_pixel = 10;
    state.camera->max_bounces = 10;
    state.camera->background = Vec3(0.70, 0.80, 1.00);
    state.camera->setup();
    state.camera->look_from_at({ 13,2,-3 }, { 0,0,0 });
}

void init_scene_quads()
{
    auto left_red     = make_shared<LambertianMaterial>(Vec3(1.0, 0.2, 0.2));
    auto back_green   = make_shared<LambertianMaterial>(Vec3(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<LambertianMaterial>(Vec3(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<LambertianMaterial>(Vec3(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<LambertianMaterial>(Vec3(0.2, 0.8, 0.8));
    
    state.scene->objects.push_back(
        make_shared<Quad>( Vec3(-3,-2, 5), Vec3(0, 0,-4), Vec3(0, 4, 0), left_red)
    );
    state.scene->objects.push_back(
        make_shared<Quad>( Vec3(-2,-2, 5), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green)
    );
    state.scene->objects.push_back(
        make_shared<Quad>( Vec3( 3,-2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue)
    );
    state.scene->objects.push_back(
        make_shared<Quad>( Vec3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange)
    );
    state.scene->objects.push_back(
        make_shared<Quad>( Vec3(-2,-3, 5), Vec3(4, 0, 0), Vec3(0, 0,-4), lower_teal)
    );
    
    state.scene->objects.push_back(
        make_shared<Triangle>( Vec3(-1,-1,0), Vec3(1,0,0), Vec3(0,1,0), left_red)
    );
    state.scene->objects.push_back(
        make_shared<Disk>( 0.5, Vec3(0.6,-1,0), Vec3(1,0,0), Vec3(0,1,0), lower_teal)
    );
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 80;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.0;
    state.camera->samples_per_pixel = 10;
    state.camera->max_bounces = 10;
    state.camera->background = Vec3(0.70, 0.80, 1.00);
    state.camera->setup();
    state.camera->look_from_at({ 0,0, -3 }, { 0,0,0 });
}

void init_scene_light()
{
    
    auto perlin_texture = make_shared<PerlinTexture>(4);
    state.scene->objects.push_back(
        make_shared<Sphere>(Vec3(0,-1000,0), 1000, make_shared<LambertianMaterial>(perlin_texture))
    );
    state.scene->objects.push_back(
        make_shared<Sphere>(Vec3(0,2,0), 2, make_shared<LambertianMaterial>(perlin_texture))
    );

    auto diffuse_light = make_shared<DiffuseLightMaterial>(Vec3(4,4,4));
    state.scene->objects.push_back(
        make_shared<Quad>(Vec3(3,1,2), Vec3(2,0,0), Vec3(0,2,0), diffuse_light)
    );
    state.scene->objects.push_back(
        make_shared<Sphere>(Vec3(0,7,0), 2, diffuse_light)
    );
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 20;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.0;
    state.camera->samples_per_pixel = 10;
    state.camera->max_bounces = 10;
    state.camera->background = Vec3(0,0,0);
    state.camera->setup();
    state.camera->look_from_at({ 26,3,-6 }, { 0,2,0 });
}

void init_scene_cornell_box() {

    auto red   = make_shared<LambertianMaterial>   (Vec3(.65, .05, .05));
    auto white = make_shared<LambertianMaterial>   (Vec3(.73, .73, .73));
    auto green = make_shared<LambertianMaterial>   (Vec3(.12, .45, .15));
    auto light = make_shared<DiffuseLightMaterial> (Vec3(15, 15, 15));

    state.scene->objects.push_back(
        make_shared<Quad>(Vec3(0,0,0), Vec3(0,555,0), Vec3(0,0,555), green)
    );
    state.scene->objects.push_back(
        make_shared<Quad>(Vec3(555,0,0), Vec3(0,555,0), Vec3(0,0,555), red)
    );
    state.scene->objects.push_back(
        make_shared<Quad>(Vec3(343, 554, 332), Vec3(-130,0,0), Vec3(0,0,-105), light)
    );
    state.scene->objects.push_back(
        make_shared<Quad>(Vec3(0,0,0), Vec3(555,0,0), Vec3(0,0,555), white)
    );
    state.scene->objects.push_back(
        make_shared<Quad>(Vec3(555,555,555), Vec3(-555,0,0), Vec3(0,0,-555), white)
    );
    state.scene->objects.push_back(
        make_shared<Quad>(Vec3(0,0,555), Vec3(555,0,0), Vec3(0,555,0), white)
    );
    
//    auto box1 = box(Vec3(130, 0, 65), Vec3(295, 165, 230), white);
//    auto box2 = box(Vec3(265, 0, 295), Vec3(430, 330, 460), white);
//    state.scene->objects.insert( state.scene->objects.end(), box1.begin(), box1.end() );
//    state.scene->objects.insert( state.scene->objects.end(), box2.begin(), box2.end() );
    
    vector<shared_ptr<Hittable>> b1 = box(Vec3(0,0,0), Vec3(165,330,165), white);
    shared_ptr<Hittable> box1 = std::make_shared<HittableList>(b1);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265,0,295));
    state.scene->objects.push_back(box1);
    
    vector<shared_ptr<Hittable>> b2 = box(Vec3(0,0,0), Vec3(165,165,165), white);
    shared_ptr<Hittable> box2 = std::make_shared<HittableList>(b2);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130,0,65));
    state.scene->objects.push_back(box2);
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 40;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.0;
    state.camera->samples_per_pixel = 30;
    state.camera->max_bounces = 10;
    state.camera->background = Vec3(0,0,0);
    state.camera->setup();
    state.camera->look_from_at({ 278, 278, -800 }, { 278, 278, 0 });
    
}
