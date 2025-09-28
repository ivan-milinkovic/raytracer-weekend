#include <memory>
using std::make_shared;
#include "image.h"
#include "camera.h"
#include "sphere.h"
#include "quad.h"
#include "scene.h"
#include "rwimage.h"
#include "hittable_list.h"
#include "constant_medium.h"
#include "hit_polymorph.h"

// viewport - A projection plane in 3D space. In world space, not view space:
//            because objects are not projected, not transformed to view space.
//            Instead, the camera is moved and it generates rays from it's own transform.
// screen - an image on monitor (screen space)


class State {
public:
    std::unique_ptr<Image> image;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Camera> camera;
    void (*render_pass_callback)(RawImage&);
    void (*render_progress_callback)(double);
    // separate from Image, because swift calls destructors on classes
    // causing double deletes
    RawImage raw_image;
    
    double screen_aspect;
    int screen_W;
    int screen_H;
    
    ~State() {
        free(raw_image.bytes);
    }
};

State state = State();

Image* rw_get_image() {
    return state.image.get();
}

RawImage& rw_get_raw_image() {
    return state.raw_image;
}

void rw_set_render_pass_callback(void (*render_pass_callback)(RawImage&)) {
    state.render_pass_callback = render_pass_callback;
}

void rw_set_render_progress_callback(void (*render_progress_callback)(double)) {
    state.render_progress_callback = render_progress_callback;
}

void init(int scene_id);
void init_scene_3_balls();
void init_scene_bouncing_balls();
void init_scene_texture();
void init_scene_perlin_spheres();
void init_scene_quads();
void init_scene_light();
void init_scene_cornell_box();
void init_scene_cornell_smoke();
void init_scene_book_2();
void render();

void rwmain(int scene_id)
{
    init(scene_id);
    
    auto t0 = std::chrono::high_resolution_clock::now();
    
    render();
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << dt << "ms\n";
}

void init(int scene_id) {
    switch(scene_id) {
        case 1: init_scene_bouncing_balls(); break;
        case 2: init_scene_book_2(); break;
        case 3: init_scene_3_balls(); break;
        case 4: init_scene_texture(); break;
        case 5: init_scene_perlin_spheres(); break;
        case 6: init_scene_quads(); break;
        case 7: init_scene_light(); break;
        case 8: init_scene_cornell_box(); break;
        case 9: init_scene_cornell_smoke(); break;
        default: printf("unknown scene id %d", scene_id); break;
    }
}

void init_image(int width, double aspect) {
    state.screen_aspect = aspect;
    state.screen_W = width;
    state.screen_H = (int) (state.screen_W / state.screen_aspect);
    state.image = std::make_unique<Image>(state.screen_W, state.screen_H);
    state.scene = std::make_unique<Scene>();
    
    state.raw_image.bytes = (uint8_t*) malloc(state.image->W() * state.image->H());
    state.raw_image.w = state.image->W();
    state.raw_image.h = state.image->H();
    state.raw_image.pixel_size = state.image->pixel_size;
}

inline void render() {
    state.camera->render(*state.scene, state.raw_image, *state.image, *state.render_pass_callback, *state.render_progress_callback);
}

void init_scene_3_balls()
{
    init_image(600, 16 / 9.0);
    
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
    
    state.scene->add( center );
    state.scene->add( left );
    state.scene->add( bubble );
    state.scene->add( right );
    state.scene->add( ground );
    
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

void init_scene_bouncing_balls()
{
    init_image(600, 16 / 9.0);
    
    // auto material_ground = make_shared<LambertianMaterial>( Vec3(0.5, 0.5, 0.5) );
    auto texture_checker = make_shared<CheckerTexture>(0.32, Vec3(0.05, 0.05, .4), Vec3(.9, .9, .9));
    auto material_ground = make_shared<LambertianMaterial> ( texture_checker );
    
    auto material1 = make_shared<DielectricMaterial> ( 1.5 );
    auto material2 = make_shared<LambertianMaterial> ( Vec3(0.4, 0.2, 0.1) );
    auto material3 = make_shared<MetalMaterial> ( Vec3(0.7, 0.6, 0.5), 0.0 );

    state.scene->add( make_shared<Sphere>( Vec3(0, -1000, 3), 1000, material_ground) );
    state.scene->add( make_shared<Sphere>( Vec3(0, 1, 0), 1, material1) );
    state.scene->add( make_shared<Sphere>( Vec3(-4, 1, 0), 1, material2) );
    state.scene->add( make_shared<Sphere>( Vec3(4, 1, 0), 1, material3) );
    
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
                    state.scene->add( make_shared<Sphere>( center, center2, 0.2, mat) );
                }
                else if (mat_dist < 0.95)
                {
                    Vec3 color = Vec3::random(0.5, 1);
                    double fuzz = rw_random(0, 0.5);
                    auto mat = make_shared<MetalMaterial>( color, fuzz );
                    state.scene->add( make_shared<Sphere>( center, 0.2, mat) );
                }
                else
                {
                    auto mat = make_shared<DielectricMaterial> ( 1.5 );
                    state.scene->add( make_shared<Sphere>( center, 0.2, mat) );
                }
            }
        }
    }
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 20;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.6;
    state.camera->samples_per_pixel = 24;
    state.camera->max_bounces = 10;
    state.camera->background = Vec3(0.70, 0.80, 1.00);
    state.camera->setup();
    state.camera->look_from_at({ 13,2,3 }, { 0,0,0 });
}

void init_scene_texture()
{
    init_image(600, 16 / 9.0);
    
    auto file_path = root_dir() / "res" / "tex.png";
    auto texture = make_shared<ImageTexture>(file_path.string().c_str());
    state.scene->add(
        make_shared<Sphere>(Vec3(0, 0, 0), 2, make_shared<LambertianMaterial>(texture))
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
    init_image(600, 16 / 9.0);
    
    auto perlin_texture = make_shared<PerlinTexture>(4);
    state.scene->add(
        make_shared<Sphere>(Vec3(0,-1000,0), 1000, make_shared<LambertianMaterial>(perlin_texture))
    );
    state.scene->add(
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
    init_image(600, 16 / 9.0);
    
    auto left_red     = make_shared<LambertianMaterial>(Vec3(1.0, 0.2, 0.2));
    auto back_green   = make_shared<LambertianMaterial>(Vec3(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<LambertianMaterial>(Vec3(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<LambertianMaterial>(Vec3(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<LambertianMaterial>(Vec3(0.2, 0.8, 0.8));
    
    state.scene->add(
        make_shared<Quad>( Vec3(-3,-2, 5), Vec3(0, 0,-4), Vec3(0, 4, 0), left_red)
    );
    state.scene->add(
        make_shared<Quad>( Vec3(-2,-2, 5), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green)
    );
    state.scene->add(
        make_shared<Quad>( Vec3( 3,-2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue)
    );
    state.scene->add(
        make_shared<Quad>( Vec3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange)
    );
    state.scene->add(
        make_shared<Quad>( Vec3(-2,-3, 5), Vec3(4, 0, 0), Vec3(0, 0,-4), lower_teal)
    );
    
    state.scene->add(
        make_shared<Triangle>( Vec3(-1,-1,0), Vec3(1,0,0), Vec3(0,1,0), left_red)
    );
    state.scene->add(
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
    init_image(600, 16 / 9.0);
    
    auto perlin_texture = make_shared<PerlinTexture>(4);
    state.scene->add(
        make_shared<Sphere>(Vec3(0,-1000,0), 1000, make_shared<LambertianMaterial>(perlin_texture))
    );
    state.scene->add(
        make_shared<Sphere>(Vec3(0,2,0), 2, make_shared<LambertianMaterial>(perlin_texture))
    );

    auto diffuse_light = make_shared<DiffuseLightMaterial>(Vec3(4,4,4));
    state.scene->add(
        make_shared<Quad>(Vec3(3,1,2), Vec3(2,0,0), Vec3(0,2,0), diffuse_light)
    );
    state.scene->add(
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

void init_scene_cornell_box()
{
    init_image(600, 16 / 9.0);
    
    auto red   = make_shared<LambertianMaterial>   (Vec3(.65, .05, .05));
    auto white = make_shared<LambertianMaterial>   (Vec3(.73, .73, .73));
    auto green = make_shared<LambertianMaterial>   (Vec3(.12, .45, .15));
    auto light = make_shared<DiffuseLightMaterial> (Vec3(15, 15, 15));

    state.scene->add(
        make_shared<Quad>(Vec3(0,0,0), Vec3(0,555,0), Vec3(0,0,555), green)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(555,0,0), Vec3(0,555,0), Vec3(0,0,555), red)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(343, 554, 332), Vec3(-130,0,0), Vec3(0,0,-105), light)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(0,0,0), Vec3(555,0,0), Vec3(0,0,555), white)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(555,555,555), Vec3(-555,0,0), Vec3(0,0,-555), white)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(0,0,555), Vec3(555,0,0), Vec3(0,555,0), white)
    );
    
    vector<shared_ptr<Hittable>> b1 = make_box(Vec3(0,0,0), Vec3(165,330,165), white);
    shared_ptr<Hittable> box1 = std::make_shared<HittableList>(b1);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265,0,295));
    state.scene->add(box1);
    
    vector<shared_ptr<Hittable>> b2 = make_box(Vec3(0,0,0), Vec3(165,165,165), white);
    shared_ptr<Hittable> box2 = std::make_shared<HittableList>(b2);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130,0,65));
    state.scene->add(box2);
    
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

void init_scene_cornell_smoke()
{
    init_image(600, 16 / 9.0);
    
    auto red   = make_shared<LambertianMaterial>   (Vec3(.65, .05, .05));
    auto white = make_shared<LambertianMaterial>   (Vec3(.73, .73, .73));
    auto green = make_shared<LambertianMaterial>   (Vec3(.12, .45, .15));
    auto light = make_shared<DiffuseLightMaterial> (Vec3(15, 15, 15));

    state.scene->add(
        make_shared<Quad>(Vec3(0,0,0), Vec3(0,555,0), Vec3(0,0,555), green)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(555,0,0), Vec3(0,555,0), Vec3(0,0,555), red)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(343, 554, 332), Vec3(-130,0,0), Vec3(0,0,-105), light)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(0,0,0), Vec3(555,0,0), Vec3(0,0,555), white)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(555,555,555), Vec3(-555,0,0), Vec3(0,0,-555), white)
    );
    state.scene->add(
        make_shared<Quad>(Vec3(0,0,555), Vec3(555,0,0), Vec3(0,555,0), white)
    );
    
    vector<shared_ptr<Hittable>> b1 = make_box(Vec3(0,0,0), Vec3(165,330,165), white);
    shared_ptr<Hittable> box1 = std::make_shared<HittableList>(b1);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265,0,295));
    auto smoke_box_1 = make_shared<ConstantMedium>(box1, 0.01, Vec3(0,0,0));
    state.scene->add(smoke_box_1);
    
    vector<shared_ptr<Hittable>> b2 = make_box(Vec3(0,0,0), Vec3(165,165,165), white);
    shared_ptr<Hittable> box2 = std::make_shared<HittableList>(b2);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130,0,65));
    auto smoke_box_2 = make_shared<ConstantMedium>(box2, 0.01, Vec3(1,1,1));
    state.scene->add(smoke_box_2);
    
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


void init_scene_book_2()
{
    init_image(600, 1.0);
    
    auto light_material = make_shared<DiffuseLightMaterial>(Vec3(7, 7, 7));
    state.scene->add(make_shared<Quad>(Vec3(123,554,147), Vec3(300,0,0), Vec3(0,0,265), light_material));
    
    auto boxes1 = make_shared<HittableList>();
    auto ground_material = make_shared<LambertianMaterial>(Vec3(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = rw_random(1, 101);
            auto z1 = z0 + w;
            
            boxes1->add(make_box_2(Vec3(x0,y0,z0), Vec3(x1,y1,z1), ground_material));
        }
    }
    state.scene->add(boxes1);
    
    // top right sphere
    auto center1 = Vec3(400, 400, 200);
    auto center2 = center1 + Vec3(30,0,0);
    auto sphere_material = make_shared<LambertianMaterial>(Vec3(0.7, 0.3, 0.1));
    state.scene->add(make_shared<Sphere>(center1, center2, 50, sphere_material));
    
    // bottom middle sphere
    state.scene->add(make_shared<Sphere>(Vec3(260, 150, 45), 50,
                                         make_shared<DielectricMaterial>(1.5)));
    
    // bottom left sphere
    state.scene->add(make_shared<Sphere>(Vec3(0, 150, 145), 50,
                                         make_shared<MetalMaterial>(Vec3(0.8, 0.8, 0.9), 1.0)));
    
    // bottom right sphere
    auto boundary = make_shared<Sphere>(Vec3(360,150,145), 70, make_shared<DielectricMaterial>(1.5));
    state.scene->add(boundary);
    
    // full scene volumetric
//    state.scene->add(make_shared<ConstantMedium>(boundary, 0.2, Vec3(0.2, 0.4, 0.9)));
//    boundary = make_shared<Sphere>(Vec3(0,0,0), 5000, make_shared<DielectricMaterial>(1.5));
//    state.scene->add(make_shared<ConstantMedium>(boundary, .0001, Vec3(1,1,1)));

    // left middle textured sphere
    auto file_path = root_dir() / "res" / "tex.png";
    auto texture_mat = make_shared<LambertianMaterial>(make_shared<ImageTexture>(file_path.string().c_str()));
    state.scene->add(make_shared<Sphere>(Vec3(400,200,400), 100, texture_mat));
    
    // middle noise sphere
    auto perlin_texture = make_shared<PerlinTexture>(0.2);
    state.scene->add(make_shared<Sphere>(Vec3(220,280,300), 80, make_shared<LambertianMaterial>(perlin_texture)));

    // bubbles
    HittableList boxes2;
    auto white = make_shared<LambertianMaterial>(Vec3(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<Sphere>(Vec3::random(0,165), 10, white));
    }
    state.scene->add(make_shared<Translate>(
        make_shared<RotateY>(make_shared<BVH_Node>(boxes2), 15),
        Vec3(-100,270,395)
        )
    );
    
    state.scene->make_bvh();
    
    state.camera = std::make_unique<Camera>(state.screen_W, state.screen_H);
    state.camera->vfov_deg = 40;
    state.camera->focus_dist = 10;
    state.camera->defocus_angle = 0.0;
    state.camera->samples_per_pixel = 10; // 1 - 5s, 10 - 36s, 60 - 5min
    state.camera->max_bounces = 10;
    state.camera->background = Vec3(0,0,0);
    state.camera->setup();
    state.camera->look_from_at({ 478, 278, -600 }, { 278, 278, 0 });
}
