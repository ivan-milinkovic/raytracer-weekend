#ifndef scene_bouncing_balls_h
#define scene_bouncing_balls_h

#include "scene_includes.h"

std::unique_ptr<Scene> init_scene_bouncing_balls(int screen_w, int screen_h)
{
    auto scene = std::make_unique<Scene>();
    
    scene->arena = std::make_unique<Arena>(1024);
    
    // auto material_ground = make_shared<LambertianMaterial>( Vec3(0.5, 0.5, 0.5) );
    auto texture_checker = make_shared<CheckerTexture>(0.32, Vec3(0.05, 0.05, .4), Vec3(.9, .9, .9));
    auto material_ground = make_shared<LambertianMaterial> ( texture_checker );
    
    auto material1 = make_shared<DielectricMaterial> ( 1.5 );
    auto material2 = make_shared<LambertianMaterial> ( Vec3(0.4, 0.2, 0.1) );
    auto material3 = make_shared<MetalMaterial> ( Vec3(0.7, 0.6, 0.5), 0.0 );

    scene->add( make_shared<Sphere>( Vec3(0, -1000, 3), 1000, material_ground) );
    scene->add( make_shared<Sphere>( Vec3(0, 1, 0), 1, material1) );
    scene->add( make_shared<Sphere>( Vec3(-4, 1, 0), 1, material2) );
    scene->add( make_shared<Sphere>( Vec3(4, 1, 0), 1, material3) );
    
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
                    scene->add( make_shared<Sphere>( center, center2, 0.2, mat) );
                }
                else if (mat_dist < 0.95)
                {
                    Vec3 color = Vec3::random(0.5, 1);
                    double fuzz = rw_random(0, 0.5);
                    auto mat = make_shared<MetalMaterial>( color, fuzz );
                    scene->add( make_shared<Sphere>( center, 0.2, mat) );
                }
                else
                {
                    auto mat = make_shared<DielectricMaterial> ( 1.5 );
                    scene->add( make_shared<Sphere>( center, 0.2, mat) );
                }
            }
        }
    }
    
    scene->make_bvh();
    
    auto camera = std::make_unique<Camera>(screen_w, screen_h);
    camera->vfov_deg = 20;
    camera->focus_dist = 10;
    camera->defocus_angle = 0.6;
    camera->samples_per_pixel = 24;
    camera->max_bounces = 10;
    camera->background = Vec3(0.70, 0.80, 1.00);
    camera->setup();
    camera->look_from_at({ 13,2,3 }, { 0,0,0 });
    
    scene->camera = std::move(camera);
    
    return scene;
}

#endif /* scene_bouncing_balls_h */
