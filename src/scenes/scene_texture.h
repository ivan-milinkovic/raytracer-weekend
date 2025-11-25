#ifndef scene_texture_h
#define scene_texture_h

#include "scene_includes.h"

std::unique_ptr<Scene> init_scene_texture(int screen_w, int screen_h)
{
    auto scene = std::make_unique<Scene>();
    
    scene->arena = std::make_unique<Arena>(1024);
    
    
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
    
    auto texture = make_shared<ImageTexture>(file_path.string().c_str());
    scene->add(
        make_shared<Sphere>(Vec3(0, 0, 0), 2, make_shared<LambertianMaterial>(texture))
    );
    
    scene->make_bvh();
    
    auto camera = std::make_unique<Camera>(screen_w, screen_h);
    camera->vfov_deg = 20;
    camera->focus_dist = 10;
    camera->defocus_angle = 0.0;
    camera->samples_per_pixel = 20;
    camera->max_bounces = 10;
    camera->background = Vec3(0.70, 0.80, 1.00);
    camera->setup();
    camera->look_from_at({ 0,0,-12 }, { 0,0,0 });
    
    scene->camera = std::move(camera);
    
    return scene;
}

#endif /* scene_texture_h */
