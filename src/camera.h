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

class Camera {
public:
    Vec3 camera_pos = { 0, 0, 0 };
    Vec3 camera_fwd   = { 0, 0, 1 };
    Vec3 camera_right = { 1, 0, 0 };
    Vec3 camera_up    = { 0, 1, 0 };
    Vec3 p00; // top left point in viewport
    Vec3 point_delta; // pixel size in viewport
    
    // min and max distances for ray-geometry intersections
    double ray_hit_min = 0.005;
    double ray_hit_max = 200;
    int max_bounces = 10;
    
    // multi-sampling
    int samples_per_pixel = 10;
    double samples_per_pixel_inv = 1.0 / samples_per_pixel;

    Camera(int screen_W, int screen_H) {
        
        double real_aspect = screen_W / (double) screen_H;
        double focal_len = 1.0;
        double viewport_W = 2.0;
        double viewport_H = viewport_W / real_aspect;
        
        point_delta[0] = viewport_W / (double) screen_W;
        point_delta[1] = viewport_H / (double) screen_H;
        point_delta[2] = 0;
        
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
            // The simple diffuse model - reflect uniformly around a hemisphere
            // Ray reflected_ray = Ray( hit.p, random_vec3_on_hemisphere(hit.n) );
            
            // True Lambertian Reflection - more rays closer to the normal
            // Imagine a unit sphere above p: centered at p + n, tangent to / touching p
            // Make a random point on the unit sphere above p: ps = p + (n + random_unit)
            // Reflecton dir: ps - p = (p + (n + rs)) - p = n + rs
            // Distribution is denser around normal, as the sphere is lifted above p
            // Since the sphere is touching p, no rays can go inside (through p), only outward
            Vec3 reflected_dir = norm(hit.n + Vec3::random(-1, 1));
            Ray reflected_ray = Ray( hit.p, reflected_dir );
            color = 0.5 * ray_color(reflected_ray, bounce_num-1, scene);
            
            // print_csv(hit.n);
            // print_csv(hit.p, hit.p + hit.n);
        }
        else // background
        {
            double f = (ray.Dir().Y() + 1.0) * 0.5;
            color = ((1-f) * Vec3(1, 1, 1)) + (f * Vec3(0, 0.3, 0.8));
        }
        return color;
    }
};

#endif /* camera_h */
