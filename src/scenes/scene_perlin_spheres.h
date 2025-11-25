#ifndef scene_perlin_spheres_h
#define scene_perlin_spheres_h

#include "scene_includes.h"

std::unique_ptr<Scene> init_scene_perlin_spheres(int screen_w, int screen_h)
{
    auto scene = std::make_unique<Scene>();
    
    scene->arena = std::make_unique<Arena>(1024);
    
    auto perlin_texture = make_shared<PerlinTexture>(4);
    scene->add(
        make_shared<Sphere>(Vec3(0,-1000,0), 1000, make_shared<LambertianMaterial>(perlin_texture))
    );
    scene->add(
        make_shared<Sphere>(Vec3(0,2,0), 2, make_shared<LambertianMaterial>(perlin_texture))
    );
    
    scene->make_bvh();
    
    auto camera = std::make_unique<Camera>(screen_w, screen_h);
    camera->vfov_deg = 20;
    camera->focus_dist = 10;
    camera->defocus_angle = 0.0;
    camera->samples_per_pixel = 50;
    camera->max_bounces = 10;
    camera->background = Vec3(0.70, 0.80, 1.00);
    camera->setup();
    camera->look_from_at({ 13,2,-3 }, { 0,0,0 });
    
    scene->camera = std::move(camera);
    
    return scene;
}

#endif /* scene_perlin_spheres_h */
