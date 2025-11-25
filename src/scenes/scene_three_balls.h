#ifndef scene_three_balls_h
#define scene_three_balls_h

#include "scene_includes.h"

std::unique_ptr<Scene> init_scene_3_balls(int screen_w, int screen_h)
{
    auto scene = std::make_unique<Scene>();
    
    scene->arena = std::make_unique<Arena>(1024);
    
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
    
    scene->add( center );
    scene->add( left );
    scene->add( bubble );
    scene->add( right );
    scene->add( ground );
    
    scene->make_bvh();
    
    auto camera = std::make_unique<Camera>(screen_w, screen_h);
    camera->vfov_deg = 70;
    camera->focus_dist = 1;
    camera->defocus_angle = 0;
    camera->samples_per_pixel = 50;
    camera->setup();
    camera->look_from_at({ 0, 0, 0 }, { 0, 0, 1 });
    camera->background = Vec3(0.70, 0.80, 1.00);
    
//    camera->vfov_deg = 40;
//    camera->focus_dist = 4.7;
//    camera->defocus_angle = 4;
//    camera->setup();
//    camera->look_from_at({ -4.5,0,0.5 }, { 0,0,2 });
    
    scene->camera = std::move(camera);
    
    return scene;
}

#endif /* scene_three_balls_h */
