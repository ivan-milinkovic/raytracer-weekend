#include "ppm.h"
#include "Image.h"
#include "Ray.h"

void rwmain()
{
    // screen pixels
    double screen_aspect = 16.0 / 9.0;
    int screen_W = 200;
    int screen_H = (int) (screen_W / screen_aspect);
    Image image = Image(100, 80);
    
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
    
    // render
    
    for (int r = 0; r < image.H(); r++)
    {
        for (int c = 0; c < image.W(); c++)
        {
            Vec3 p = p0 + (camera_right * (c * point_delta_x))
                        - (camera_up * (r * point_delta_y));
            Ray ray = Ray(camera_pos, norm(p - camera_pos));
            
            int i = r * image.W() + c;
            double f = (p.Y() + 1.0) * 0.5;
            image[i] = ((1-f) * Vec3(1, 1, 1)) + (f * Vec3(0, 0.3, 0.8));
        }
    }
    
    // output
    
    print_ppm(image.Pixels(), image.W(), image.H());
}
