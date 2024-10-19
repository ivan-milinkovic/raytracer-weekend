//
//  camera.h
//  RaytracerWeekendConsole
//
//  Created by Ivan Milinkovic on 12.10.24..
//

#ifndef camera_h
#define camera_h

#include <fstream>
#include "ray.h"
#include "scene.h"
#include "geometry.h"
#include "color.h"
#include "material.h"
#include "util.h"

#include <thread>

#define PRINT_PROGRESS 1


class Camera {
public:
    double focal_len;
    double vfov_deg = 70;
    
    double focus_dist = 1; // focus plane distance
    double defocus_angle = 1; // angle variation amount through each pixel
    double defocus_radius;
    
    // min and max distances for ray-geometry intersections
    double ray_hit_min = 0.0005;
    double ray_hit_max = 200;
    int max_bounces = 10;
    
    // multi-sampling
    int samples_per_pixel = 10;
    double samples_per_pixel_inv;
    
    double viewport_H;
    double viewport_W;
    
    Vec3 p00; // top left point in viewport
    Vec3 point_delta; // pixel size in viewport
    
    Vec3 camera_pos = { 0, 0, 0 };
    Vec3 camera_fwd   = { 0, 0, 1 };
    Vec3 camera_right = { 1, 0, 0 };
    Vec3 camera_up    = { 0, 1, 0 };
    Vec3 world_up     = { 0, 1, 0 }; // upright space
    
    int screen_W;
    int screen_H;
    
    Camera(int screen_W, int screen_H) {
        this->screen_W = screen_W;
        this->screen_H = screen_H;
        setup();
    }
    
    void setup() {
        samples_per_pixel_inv = 1 / (double) samples_per_pixel;
        
        focal_len = focus_dist;
        defocus_radius = focus_dist * std::tan( rad_from_deg(defocus_angle / 2.0) );
        
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
        look_from_at(camera_pos, v_look_at);
    }
    
    void look_from_at(const Vec3& from, const Vec3& v_look_at) {
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
    
    // multi-threaded: 660ms, down from 3700ms single-threaded, 5.6x speed-up
    void render(const Scene& scene, Image& image)
    {
        // hardware_concurrency is 10 for M1 pro
        const int cores = std::thread::hardware_concurrency();
        
        // 10 x 1 performs the best on M1 pro for 600x337 image size
        const int tile_rows = cores; // std::ceil(std::sqrt(cores));
        const int tile_cols = 1;     // std::max(cores - tile_rows, 1);
        const int tile_height = image.H() / tile_rows;
        const int tile_width  = image.W() / tile_cols;
        
        std::vector<std::thread*> threads;
        for (int j = 0; j < tile_rows; j++) {
            for (int i = 0; i < tile_cols; i++) {
                
                int x_start = i * tile_width;
                int twidth = tile_width;
                if(i >= tile_rows - 1) {
                    twidth = image.W() - x_start;
                }
                
                int y_start = j * tile_height;
                int theight = tile_height;
                if(j >= tile_rows - 1) {
                    theight = image.H() - y_start;
                }
                
                // printf("(%d,%d) %d x %d \n", x_start, y_start, width, height);
                auto thread = new std::thread([this, &image, &scene, y_start, theight](){
                    render_tile(scene, image, 0, image.W(), y_start, theight);
                });
                threads.push_back(thread);
            }
        }
        
        for (int i=0; i<threads.size(); i++) {
            std::thread* t = threads[i];
            t->join();
        }
    }
    
    void render_tile(const Scene& scene, Image& image,
                     int x_start, int width,
                     int y_start, int height)
    {
        for (int row = y_start; row < y_start + height; row++)
        {
            for (int col = x_start; col < x_start + width; col++)
            {
                int i = row * image.W() + col;
                Vec3& pixel = image[i];
                
                for (int k = 0; k < samples_per_pixel; k++) {
                    Vec3 viewport_point;
                    Ray ray = this->make_ray(col, row, viewport_point);
                    pixel += this->ray_color(ray, max_bounces, scene);
                }
                pixel *= samples_per_pixel_inv; // average
                
                gamma_correct(pixel);
            }
        }
    }
    
    /*
    // Single-threaded
    void render(const Scene& scene, Image& image) {
        // return test(scene);
        
        for (int row = 0; row < image.H(); row++)
        {
            for (int col = 0; col < image.W(); col++)
            {
                int i = row * image.W() + col;
                Vec3& pixel = image[i];
                
                for (int k = 0; k < samples_per_pixel; k++) {
                    Vec3 viewport_point;
                    Ray ray = this->make_ray(col, row, viewport_point);
                    pixel += this->ray_color(ray, max_bounces, scene);
                }
                pixel *= samples_per_pixel_inv; // average
                
                gamma_correct(pixel);
            }
            
            #if PRINT_PROGRESS
            double f = std::fabs((double)row / (double) image.H());
            double r = std::floor(f/0.1) * 0.1;
            if (f > 0 && f-r < 0.002)
                std::cout << (int)(r * 100) << "% ";
            #endif
        }
        
        #if PRINT_PROGRESS
        std::cout << std::endl;
        #endif
    }
    */
    
    
    // A ray with random direction offset within a pixel square
    Ray make_ray(int x, int y, Vec3& viewport_point) {
        
        // point on the image plane
        Vec3 offset_aa = sample_unit_square();
        Vec3 point = p00;
        point +=  camera_right * (x * point_delta.X() + point_delta.X() * offset_aa.X());
        point += -camera_up    * (y * point_delta.Y() + point_delta.Y() * offset_aa.Y());
        
        // ray origin
        Vec3 origin = camera_pos;
        if (defocus_angle > 0) {
            Vec3 offset_defocus = defocus_radius * sample_unit_disk();
            origin += camera_right * offset_defocus.X();
            origin += camera_up    * offset_defocus.Y();
        }
        // origin = point; // visualize the focus plane
        double time = rw_random();
        viewport_point = point;
        Ray ray = Ray(origin, norm(point - origin), time);
        return ray;
    }
    
    // A random offset to apply to a pixel ray for AA.
    // Random point within unit square between [-0.5, -0.5] and [0.5, 0.5]
    Vec3 sample_unit_square() {
        return Vec3(rw_random() - 0.5, rw_random() - 0.5, 0);
    }
    
    Vec3 sample_unit_disk () {
        // This approach shifts probability towards the disk edge,
        // because it samples the square - more points outside of disk
        Vec3 v = Vec3(rw_random(-1, 1), rw_random(-1, 1), 0);
        if (v.len() > 1) {
            v = norm(v);
        }
        return v;
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
            double f = 0.5 * (ray.dir().Y() + 1.0);
            color = ((1-f) * Vec3(1, 1, 1)) + (f * Vec3(0.5, 0.7, 1.0));
        }
        return color;
    }
    
    void test(const Scene& scene) {
        auto path = root_dir();
        path = path / "debug" / "lines.csv";
        
        std::ofstream file(path);
        std::cout << "Writing debug csv to:\n" << path << std::endl;
        
        for (int k = 0; k < 1; k++) {
            Vec3 viewport_point;
            Ray ray = this->make_ray(screen_W / 2, screen_H / 2, viewport_point);
            Hit hit;
            // auto color = ray_color(ray, max_bounces, scene);
            scene.hit(ray, Interval(ray_hit_min, ray_hit_max), hit);
            // print_csv({ray.origin(), viewport_point, hit.p}, file);
        }
        std::cout << std::endl;
        file.close();
        return;
    }
};

#endif /* camera_h */
