#ifndef camera_h
#define camera_h

#include "ray.h"

#define PRINT_PROGRESS 0

class Camera {
public:
    
    double focal_len;
    double vfov_deg = 70;
    
    double focus_dist = 1; // focus plane distance
    double defocus_angle = 1; // angle variation amount through each pixel
    double defocus_radius;
    
    // min and max distances for ray-geometry intersections
    double ray_hit_min = 0.001;
    double ray_hit_max = infinity;
    int max_bounces = 10;
    
    // multi-sampling
    int samples_per_pixel = 10;
    double samples_per_pixel_inv;
    
    Vec3 background = Vec3::zero();
    
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
    
    Image* image;
    // separate from Image, because swift calls destructors on classes
    // causing double deletes
    RawImage raw_image;
    
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
        
        point_delta.set(0, viewport_W / (double) screen_W);
        point_delta.set(1, viewport_H / (double) screen_H);
        point_delta.set(2, 0);
        
        update_p00();
        
        init_image();
    }
    
    void init_image() {
        image = new Image(screen_W, screen_H);
        
        raw_image.bytes = (uint8_t*) malloc(image->W() * image->H() * image->pixel_size);
        raw_image.w = image->W();
        raw_image.h = image->H();
        raw_image.pixel_size = image->pixel_size;
    }
    
    ~Camera() {
        delete image;
        free(raw_image.bytes);
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
};

#endif /* camera_h */
