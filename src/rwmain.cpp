#include "Image.h"
#include "Ray.h"
#include "geometry.h"

typedef struct State {
    Image* image;
} State;

State state = State();
Image* getImage() { return state.image; }


void rwmain()
{
    // screen pixels
    double screen_aspect = 16.0 / 9.0;
    int screen_W = 600;
    int screen_H = (int) (screen_W / screen_aspect);
    Image image = Image(screen_W, screen_H);
    state.image = &image;
    
    // camera - virtual viewport
    double real_aspect = screen_W / (double) screen_H;
    double focal_len = 1.0;
    double viewport_W = 2.0;
    double viewport_H = viewport_W / real_aspect;
    double point_delta_x = viewport_W / (double) screen_W;
    double point_delta_y = viewport_H / (double) screen_H;
    
    Vec3 camera_pos = { 0, 0, 0 };
    Vec3 camera_fwd   = { 0, 0, 1 };
    Vec3 camera_right = { 1, 0, 0 };
    Vec3 camera_up    = { 0, 1, 0 };
    
    // scan from top to bottom, left to right
    Vec3 viewport_top_left =
        camera_pos
        + camera_fwd * focal_len
        - camera_right * (viewport_W/2.0)
        + camera_up * (viewport_H/2.0);
    
    // top left
    Vec3 p0 = viewport_top_left
            + camera_right * (point_delta_x / 2.0)
            - camera_up * (point_delta_y / 2.0);
    
    // world
    Vec3 sphere_c = {0, 0, 4};
    double sphere_r = 1;
    
    // render
    
    auto t0 = std::chrono::high_resolution_clock::now();
    
    for (int r = 0; r < image.H(); r++)
    {
        for (int c = 0; c < image.W(); c++)
        {
            Vec3 p = p0 + (camera_right * (c * point_delta_x))
                        - (camera_up * (r * point_delta_y));
            Ray ray = Ray(camera_pos, norm(p - camera_pos));
            
            int i = r * image.W() + c;
            Vec3& pixel = image[i];
            double d = ray_to_sphere_distance(ray, sphere_c, sphere_r);
            if (d > 0) {
                Vec3 hit_point = ray.at(d);
                Vec3 normal = norm(hit_point - sphere_c);
                pixel = 0.5* Vec3AddScalar(normal, 1.0);
            }
            else {
                double f = (p.Y() + 1.0) * 0.5;
                pixel = ((1-f) * Vec3(1, 1, 1)) + (f * Vec3(0, 0.3, 0.8));
            }
        }
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << dt << "ms\n";
}
