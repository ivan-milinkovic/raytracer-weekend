#ifndef scene_cornell_smoke_h
#define scene_cornell_smoke_h

#include "scene_includes.h"

std::unique_ptr<Scene> init_scene_cornell_smoke(int screen_w, int screen_h)
{
    auto scene = std::make_unique<Scene>();
    
    scene->arena = std::make_unique<Arena>(5*1024);
    
    auto red   = make_shared<LambertianMaterial>   (Vec3(.65, .05, .05));
    auto white = make_shared<LambertianMaterial>   (Vec3(.73, .73, .73));
    auto green = make_shared<LambertianMaterial>   (Vec3(.12, .45, .15));
    auto light = make_shared<DiffuseLightMaterial> (Vec3(15, 15, 15));

    auto ptr = scene->arena->allocate<Quad>(6);
    
    scene->add(
        new (ptr + 0) Quad(Vec3(0,0,0), Vec3(0,555,0), Vec3(0,0,555), green)
    );
    scene->add(
        new (ptr + 1) Quad(Vec3(555,0,0), Vec3(0,555,0), Vec3(0,0,555), red)
    );
    scene->add(
        new (ptr + 2) Quad(Vec3(343, 554, 332), Vec3(-130,0,0), Vec3(0,0,-105), light)
    );
    scene->add(
        new (ptr + 3) Quad(Vec3(0,0,0), Vec3(555,0,0), Vec3(0,0,555), white)
    );
    scene->add(
        new (ptr + 4) Quad(Vec3(555,555,555), Vec3(-555,0,0), Vec3(0,0,-555), white)
    );
    scene->add(
        new (ptr + 5) Quad(Vec3(0,0,555), Vec3(555,0,0), Vec3(0,555,0), white)
    );
    
    vector<Hittable*> b1 = make_box(Vec3(0,0,0), Vec3(165,330,165), white, *(scene->arena));
    Hittable* box1 = new (scene->arena->allocate<HittableList>()) HittableList(b1);
    box1 = new (scene->arena->allocate<RotateY>()) RotateY(box1, 15);
    box1 = new (scene->arena->allocate<Translate>()) Translate(box1, Vec3(265,0,295));
    auto smoke_box_1 = new (scene->arena->allocate<ConstantMedium>()) ConstantMedium(box1, 0.01, Vec3(0,0,0));
    scene->add(smoke_box_1);
    
    vector<Hittable*> b2 = make_box(Vec3(0,0,0), Vec3(165,165,165), white, *(scene->arena));
    Hittable* box2 = new (scene->arena->allocate<HittableList>()) HittableList(b2);
    box2 = new (scene->arena->allocate<RotateY>()) RotateY(box2, -18);
    box2 = new (scene->arena->allocate<Translate>()) Translate(box2, Vec3(130,0,65));
    auto smoke_box_2 = new (scene->arena->allocate<ConstantMedium>()) ConstantMedium(box2, 0.01, Vec3(1,1,1));
    scene->add(smoke_box_2);
    
    scene->make_bvh();
    
    auto camera = std::make_unique<Camera>(screen_w, screen_h);
    camera->vfov_deg = 40;
    camera->focus_dist = 10;
    camera->defocus_angle = 0.0;
    camera->samples_per_pixel = 100;
    camera->max_bounces = 10;
    // camera->background = Vec3(0,0,0);
    camera->background = Vec3(0.70, 0.80, 1.00);
    camera->setup();
    camera->look_from_at({ 278, 278, -800 }, { 278, 278, 0 });
    
    scene->camera = std::move(camera);
    
    printf("Allocation size: %zu bytes \n", scene->arena->get_allocation_size());
    
    return scene;
}

#endif /* scene_cornell_smoke_h */
