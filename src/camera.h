//
//  camera.h
//  RaytracerWeekendConsole
//
//  Created by Ivan Milinkovic on 12.10.24..
//

#ifndef camera_h
#define camera_h

class Camera {
public:
    Vec3 camera_pos = { 0, 0, 0 };
    Vec3 camera_fwd   = { 0, 0, 1 };
    Vec3 camera_right = { 1, 0, 0 };
    Vec3 camera_up    = { 0, 1, 0 };
    Vec3 p0;
    // pixel to pixel increments in viewport
    double point_delta_x;
    double point_delta_y;
    

    Camera(int screen_W, int screen_H) {
        
        double real_aspect = screen_W / (double) screen_H;
        double focal_len = 1.0;
        double viewport_W = 2.0;
        double viewport_H = viewport_W / real_aspect;
        
        point_delta_x = viewport_W / (double) screen_W;
        point_delta_y = viewport_H / (double) screen_H;
        
        Vec3 viewport_top_left =
            camera_pos
            + camera_fwd * focal_len
            - camera_right * (viewport_W/2.0)
            + camera_up * (viewport_H/2.0);
        
        // top left point in viewport representing pixel (0,0)
        p0 = viewport_top_left
           + camera_right * (point_delta_x / 2.0)
           - camera_up * (point_delta_y / 2.0);
    }
    
    Ray make_ray(int x, int y, Vec3& viewport_point) {
        Vec3 p = p0 + (camera_right * (x * point_delta_x))
                    - (camera_up * (y * point_delta_y));
        Ray ray = Ray(camera_pos, norm(p - camera_pos));
        viewport_point = p;
        return ray;
    }
};


#endif /* camera_h */
