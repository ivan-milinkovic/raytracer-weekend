//
//  camera.h
//  RaytracerWeekendConsole
//
//  Created by Ivan Milinkovic on 12.10.24..
//

#ifndef camera_h
#define camera_h

#include "ray.h"
#include "scene.h"
#include "geometry.h"
#include "color.h"
#include "material.h"
#include "util.h"

const int conf_samples_per_pixel = 10;
const int conf_max_bounces = 10;

class Camera {
public:
    double focal_len = 1.0;
    double vfov_deg = 70;
    
    // min and max distances for ray-geometry intersections
    double ray_hit_min = 0.005;
    double ray_hit_max = 200;
    int max_bounces = conf_max_bounces;
    
    // multi-sampling
    int samples_per_pixel = conf_samples_per_pixel;
    double samples_per_pixel_inv = 1.0 / samples_per_pixel;
    
    double viewport_H;
    double viewport_W;
    
    Vec3 p00; // top left point in viewport
    Vec3 point_delta; // pixel size in viewport
    
    Vec3 camera_pos = { 0, 0, 0 };
    Vec3 camera_fwd   = { 0, 0, 1 };
    Vec3 camera_right = { 1, 0, 0 };
    Vec3 camera_up    = { 0, 1, 0 };
    Vec3 world_up     = { 0, 1, 0 }; // upright space
    
    
    Camera(int screen_W, int screen_H) {
        
        double real_aspect = screen_W / (double) screen_H;
        double vfov_rad = rad_from_deg(vfov_deg);
        double h = std::tan(vfov_rad/2); // height from viewport middle to viewport top
        viewport_H = 2 * h * focal_len;
        viewport_W = viewport_H * real_aspect;
        
        point_delta[0] = viewport_W / (double) screen_W;
        point_delta[1] = viewport_H / (double) screen_H;
        point_delta[2] = 0;
        
        update_p00();
    }
    
    void look_at(const Vec3& v_look_at) {
        look_at(camera_pos, v_look_at);
    }
    
    void look_at(const Vec3& from, const Vec3& v_look_at) {
        camera_pos = Vec3(from);
        camera_fwd = norm( v_look_at - camera_pos );
        // left handed
        camera_right = norm( cross(world_up, camera_fwd) );
        camera_up = norm( cross(camera_fwd, camera_right) );
        update_p00();
    }
    
    void update_p00() {
        Vec3 viewport_top_left =
            camera_pos
            + camera_fwd * focal_len
            - camera_right * (viewport_W/2.0)
            + camera_up * (viewport_H/2.0);
        
        // top left point in viewport representing pixel (0,0)
        p00 = viewport_top_left
            + camera_right * (point_delta.X() / 2.0)
            - camera_up * (point_delta.Y() / 2.0);
    }
    
    void render(const Scene& scene, Image& image) {
        
        for (int row = 0; row < image.H(); row++)
        {
            for (int col = 0; col < image.W(); col++)
            {
                int i = row * image.W() + col;
                Vec3& pixel = image[i];
                
                // No random
                // Vec3 viewport_point;
                // Ray ray = this->make_ray(col, row, viewport_point);
                // pixel = this->ray_color(ray, scene);
                
                // random sampling within pixel size
                for (int k = 0; k < samples_per_pixel; k++) {
                    Vec3 viewport_point;
                    Ray ray = this->make_ray_msaa(col, row, viewport_point);
                    pixel += this->ray_color(ray, max_bounces, scene);
                }
                pixel *= samples_per_pixel_inv; // average
                
                gamma_correct(pixel);
            }
        }
    }
    
    Ray make_ray(int x, int y, Vec3& viewport_point) {
        Vec3 p = p00 + (camera_right * (x * point_delta.X()))
                     - (camera_up * (y * point_delta.Y()));
        Ray ray = Ray(camera_pos, norm(p - camera_pos));
        viewport_point = p;
        return ray;
    }
    
    // A ray with random direction offset within pixel size
    Ray make_ray_msaa(int x, int y, Vec3& viewport_point) {
        Vec3 offset = sample_unit_square() * point_delta;
        Vec3 p = p00 + (camera_right * (x * point_delta.X()))
                     - (camera_up * (y * point_delta.Y()))
                     + offset;
        Ray ray = Ray(camera_pos, norm(p - camera_pos));
        viewport_point = p;
        return ray;
    }
    
    
    // A random offset to apply to a pixel ray for MSAA.
    // Random point within unit square between [-0.5, -0.5] and [0.5, 0.5]
    Vec3 sample_unit_square() {
        return Vec3(rw_random() - 0.5,
                    rw_random() - 0.5,
                    0);
    }
    
    Vec3 ray_color(const Ray& ray, int bounce_num, const Scene& scene)
    {
        if (bounce_num <= 0) {
            return Vec3(0,0,0);
        }
        
        Vec3 color;
        Hit hit;
        if (scene.hit(ray, Interval(ray_hit_min, ray_hit_max), hit))
        {
            Vec3 attenuation;
            Ray scattered;
            if (hit.material->visit_scatter(ray, hit, attenuation, scattered)) {
                Vec3 bounced_color = ray_color(scattered, bounce_num-1, scene);
                color = attenuation * bounced_color;
            } else {
                color = Vec3::zero();
            }
        }
        else // background
        {
            double f = 0.5 * (ray.Dir().Y() + 1.0);
            color = ((1-f) * Vec3(1, 1, 1)) + (f * Vec3(0.5, 0.7, 1.0));
        }
        return color;
    }
};

#endif /* camera_h */
