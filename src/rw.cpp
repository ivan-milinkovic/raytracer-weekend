#include <memory>
using std::make_shared;

#if RW_APPLE_LOAD_RESOURCES_FROM_BUNDLE
#include <CoreFoundation/CFBundle.h>
#endif

#include "tracer.h"
#include "scenes.h"


// viewport - A projection plane in 3D space. In world space, not view space:
//            because objects are not projected, not transformed to view space.
//            Instead, the camera is moved and it generates rays from it's own transform.
// screen - an image on monitor (screen space)


class State {
public:
    std::unique_ptr<Tracer> tracer;
    std::unique_ptr<Scene> scene;
    int scene_id = -1;
    
    void (*render_pass_callback)(RawImage&);
    void (*render_progress_callback)(double);
};

State state = State();

Image* rw_get_image() {
    return state.scene->camera->image;
}

RawImage& rw_get_raw_image() {
    return state.scene->camera->raw_image;
}

void rw_set_render_pass_callback(void (*render_pass_callback)(RawImage&)) {
    state.render_pass_callback = render_pass_callback;
}

void rw_set_render_progress_callback(void (*render_progress_callback)(double)) {
    state.render_progress_callback = render_progress_callback;
}

void rw_init_scene(int scene_id) {
    
    auto t0 = std::chrono::high_resolution_clock::now();
    
    state.scene_id = scene_id;
    state.tracer = std::make_unique<Tracer>();
    
    double aspect_16_9 = 16.0 / 9.0;
    int screen_w = 600;
    int screen_h = (int) (600 / aspect_16_9);
    
    switch(state.scene_id) {
        case 1:
            state.scene = init_scene_bouncing_balls(screen_w, screen_h);
            break;
        case 2:
            state.scene = init_scene_3_balls(screen_w, screen_h);
            break;
        case 3:
            state.scene = init_scene_texture(screen_w, screen_h);
            break;
        case 4:
            state.scene = init_scene_perlin_spheres(screen_w, screen_h);
            break;
        case 5:
            state.scene = init_scene_quads(screen_w, screen_h);
            break;
        case 6:
            state.scene = init_scene_light(screen_w, screen_h);
            break;
        case 7:
            state.scene = init_scene_cornell_box(screen_w, screen_h);
            break;
        case 8:
            state.scene = init_scene_cornell_smoke(screen_w, screen_h);
            break;
        case 9:
            state.scene = init_scene_book_2(screen_w, screen_h);
            break;
        default: printf("unknown scene id %d", scene_id); break;
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "scene " << state.scene_id << ": init: " << dt << "ms" << std::endl;
}

void rw_render() {
    auto t0 = std::chrono::high_resolution_clock::now();
    
    state.tracer->render(*state.scene, *state.scene->camera, *state.render_pass_callback, *state.render_progress_callback);
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "scene " << state.scene_id << ": render: " << dt << "ms" << std::endl;
}
