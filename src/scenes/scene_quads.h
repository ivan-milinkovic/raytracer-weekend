#ifndef scene_quads_h
#define scene_quads_h

#include "scene_includes.h"

std::unique_ptr<Scene> init_scene_quads(int screen_w, int screen_h)
{
    auto scene = std::make_unique<Scene>();
    
    scene->arena = std::make_unique<Arena>(1024);
    
    auto left_red     = make_shared<LambertianMaterial>(Vec3(1.0, 0.2, 0.2));
    auto back_green   = make_shared<LambertianMaterial>(Vec3(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<LambertianMaterial>(Vec3(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<LambertianMaterial>(Vec3(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<LambertianMaterial>(Vec3(0.2, 0.8, 0.8));
    
    scene->add(
        make_shared<Quad>( Vec3(-3,-2, 5), Vec3(0, 0,-4), Vec3(0, 4, 0), left_red)
    );
    scene->add(
        make_shared<Quad>( Vec3(-2,-2, 5), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green)
    );
    scene->add(
        make_shared<Quad>( Vec3( 3,-2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue)
    );
    scene->add(
        make_shared<Quad>( Vec3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange)
    );
    scene->add(
        make_shared<Quad>( Vec3(-2,-3, 5), Vec3(4, 0, 0), Vec3(0, 0,-4), lower_teal)
    );
    
    scene->add(
        make_shared<Triangle>( Vec3(-1,-1,0), Vec3(1,0,0), Vec3(0,1,0), left_red)
    );
    scene->add(
        make_shared<Disk>( 0.5, Vec3(0.6,-1,0), Vec3(1,0,0), Vec3(0,1,0), lower_teal)
    );
    
    scene->make_bvh();
    
    auto camera = std::make_unique<Camera>(screen_w, screen_h);
    camera->vfov_deg = 80;
    camera->focus_dist = 10;
    camera->defocus_angle = 0.0;
    camera->samples_per_pixel = 50;
    camera->max_bounces = 10;
    camera->background = Vec3(0.70, 0.80, 1.00);
    camera->setup();
    camera->look_from_at({ 0,0, -3 }, { 0,0,0 });
    
    scene->camera = std::move(camera);
    
    return scene;
}

#endif /* scene_quads_h */
