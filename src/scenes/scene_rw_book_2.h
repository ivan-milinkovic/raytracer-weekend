#ifndef scene_rw_book_2_h
#define scene_rw_book_2_h

#include "scene_includes.h"

std::unique_ptr<Scene> init_scene_book_2(int screen_w, int screen_h)
{
    auto scene = std::make_unique<Scene>();
    
    scene->arena = std::make_unique<Arena>(1024);
    
    auto light_material = make_shared<DiffuseLightMaterial>(Vec3(7, 7, 7));
    scene->add(make_shared<Quad>(Vec3(123,554,147), Vec3(300,0,0), Vec3(0,0,265), light_material));
    
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
    scene->add(boxes1);
    
    // top right sphere
    auto center1 = Vec3(400, 400, 200);
    auto center2 = center1 + Vec3(30,0,0);
    auto sphere_material = make_shared<LambertianMaterial>(Vec3(0.7, 0.3, 0.1));
    scene->add(make_shared<Sphere>(center1, center2, 50, sphere_material));
    
    // bottom middle sphere
    scene->add(make_shared<Sphere>(Vec3(260, 150, 45), 50,
                                         make_shared<DielectricMaterial>(1.5)));
    
    // bottom left sphere
    scene->add(make_shared<Sphere>(Vec3(0, 150, 145), 50,
                                         make_shared<MetalMaterial>(Vec3(0.8, 0.8, 0.9), 1.0)));
    
    // bottom right sphere
    auto boundary = make_shared<Sphere>(Vec3(360,150,145), 70, make_shared<DielectricMaterial>(1.5));
    scene->add(boundary);
    
    // full scene volumetric
//    scene->add(make_shared<ConstantMedium>(boundary, 0.2, Vec3(0.2, 0.4, 0.9)));
//    boundary = make_shared<Sphere>(Vec3(0,0,0), 5000, make_shared<DielectricMaterial>(1.5));
//    scene->add(make_shared<ConstantMedium>(boundary, .0001, Vec3(1,1,1)));

    // left middle textured sphere
#if RW_APPLE_LOAD_RESOURCES_FROM_BUNDLE
    CFURLRef resourceURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
    char resourcePath[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourceURL, true, (UInt8 *)resourcePath, PATH_MAX)) {
        printf("cannot load from bundle");
        return scene;
    }
    if (resourceURL != NULL) CFRelease(resourceURL);
    
    std::filesystem::path file_path(resourcePath);
    file_path = file_path / "tex.png";
#else
    std::filesystem::path file_path = root_dir() / "res" / "tex.png";
#endif
    auto texture_mat = make_shared<LambertianMaterial>(make_shared<ImageTexture>(file_path.string().c_str()));
    scene->add(make_shared<Sphere>(Vec3(400,200,400), 100, texture_mat));
    
    // middle noise sphere
    auto perlin_texture = make_shared<PerlinTexture>(0.2);
    scene->add(make_shared<Sphere>(Vec3(220,280,300), 80, make_shared<LambertianMaterial>(perlin_texture)));

    // bubbles
    HittableList boxes2;
    auto white = make_shared<LambertianMaterial>(Vec3(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<Sphere>(Vec3::random(0,165), 10, white));
    }
    scene->add(make_shared<Translate>(
        make_shared<RotateY>(make_shared<BVH_Node>(boxes2), 15),
        Vec3(-100,270,395)
        )
    );
    
    scene->make_bvh();
    
    auto camera = std::make_unique<Camera>(screen_w, screen_h);
    camera->vfov_deg = 40;
    camera->focus_dist = 10;
    camera->defocus_angle = 0.0;
    camera->samples_per_pixel = 10; // 1 - 5s, 10 - 36s, 60 - 5min
    camera->max_bounces = 10;
    camera->background = Vec3(0,0,0);
    camera->setup();
    camera->look_from_at({ 478, 278, -600 }, { 278, 278, 0 });
    
    scene->camera = std::move(camera);
    
    return scene;
}

#endif /* scene_rw_book_2_h */
