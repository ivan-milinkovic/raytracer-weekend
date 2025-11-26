#ifndef tracer_h
#define tracer_h

#include <thread>
#include <latch>
#include "math/vec3.h"
#include "img/image.h"
#include "scene.h"
#include "camera.h"
#include "util/thread_pool.h"
#include "material.h"

class Tracer {
    
public:
    
    // In this version, the multisampling loop is the outer loop
    // allowing for callbacks when each multisample render pass ends
    void render(const Scene& scene,
                Camera& camera,
                void (*render_pass_callback)(RawImage&),
                void (*render_progress_callback)(double)
                )
    {
        // return test(scene);
        
        camera.image->zero();
        
        // hardware_concurrency is 10 for M1 pro
        const int cores = std::thread::hardware_concurrency();
        
        // 10 x 1 performs the best on M1 pro for 600x337 image size
        const int tile_rows = cores; // std::floor(std::sqrt(cores));
        const int tile_cols = 1;     // tile_rows;
        
        const int tile_height = camera.image->H() / tile_rows;
        const int tile_width  = camera.image->W() / tile_cols;
        
        int tile_id = 0;
        
        #if PRINT_PROGRESS
        printf("%d tiles\n", tile_rows*tile_cols);
        #endif
        
        std::atomic_int progress(0);
        const int totalProgress = tile_rows * tile_cols * camera.samples_per_pixel;
        
        ThreadPool thread_pool(cores, RWThreadPriority::high);
        
        // multisampling
        for (int k = 0; k < camera.samples_per_pixel; k++) {
            
            std::latch countdown(tile_rows * tile_cols);
            
            for (int j = 0; j < tile_rows; j++) {
                for (int i = 0; i < tile_cols; i++) {
                    
                    int x_start = i * tile_width;
                    int twidth = tile_width;
                    if(i >= tile_rows - 1) {
                        twidth = camera.image->W() - x_start;
                    }
                    
                    int y_start = j * tile_height;
                    int theight = tile_height;
                    if(j >= tile_rows - 1) {
                        theight = camera.image->H() - y_start;
                    }
                    
                    int tid = ++tile_id;
                    thread_pool.enqueue([this, &scene, &camera, y_start, twidth, theight, tid, &progress, render_progress_callback, totalProgress, &countdown] () {
                        
                        this->render_tile(scene, camera, 0, twidth, y_start, theight, tid);
                        countdown.count_down();
                        
                        if (render_progress_callback) {
                            progress++;
                            render_progress_callback( ((double) progress) / ((double) totalProgress) );
                        }
                    });
                }
            }
            
            countdown.wait();
            
            // Images are incomplete without joining all the pool threads because threads do not synchronize their cache with RAM
            // https://vorbrodt.blog/2019/02/21/memory-barriers-and-thread-synchronization/
            
            // callback outside to notify that one pass is done
            if (render_pass_callback) {
                camera.image->copy_for_output_with_gamma(camera.raw_image, 1.0f/((double)(k+1))); // don't divide by zero
                render_pass_callback(camera.raw_image);
            }
        }
        
        thread_pool.stop();
        thread_pool.join();
        
        for(int i = 0; i < camera.image->W() * camera.image->H(); i++) {
            Vec3& pixel = (*camera.image)[i];
            pixel *= camera.samples_per_pixel_inv; // average
            gamma_correct(pixel);
        }
        
        camera.image->copy_for_output(camera.raw_image, 1.0f);
        
        if (render_pass_callback) {
            render_pass_callback(camera.raw_image);
        }
    }
    
    void render_tile(const Scene& scene,
                     Camera& camera,
                     int x_start, int width,
                     int y_start, int height,
                     int tile_id)
    {
        for (int row = y_start; row < y_start + height; row++)
        {
            for (int col = x_start; col < x_start + width; col++)
            {
                int i = row * camera.image->W() + col;
                Vec3& pixel = (*camera.image)[i];
                
                Vec3 viewport_point;
                Ray ray = camera.make_ray(col, row, viewport_point);
                pixel += ray_color(ray, camera.max_bounces, scene, camera);
            }
        }
        #if PRINT_PROGRESS
        printf("tile %d done\n", tile_id);
        #endif
    }
    
    Vec3 ray_color(const Ray& ray, int bounce_num, const Scene& scene, Camera& camera)
    {
        if (bounce_num <= 0) {
            return Vec3(0,0,0);
        }
        
        Hit hit;
        if (!scene.hit(ray, Interval(camera.ray_hit_min, camera.ray_hit_max), hit)) {
            return camera.background;
            // double f = 0.5 * (ray.dir().Y() + 1.0);
            // return ((1-f) * Vec3(1, 1, 1)) + (f * Vec3(0.5, 0.7, 1.0));
        }
        
        Vec3 attenuation;
        Ray scattered;
        Vec3 emission_color = hit.material->visit_emitted(hit.u, hit.v, hit.p);
        if (!hit.material->visit_scatter(ray, hit, attenuation, scattered)) {
            return emission_color;
        }
        
        Vec3 bounced_color = ray_color(scattered, bounce_num-1, scene, camera);
        Vec3 scatter_color = attenuation * bounced_color;
        
        return emission_color + scatter_color;
    }
    
    // needs #include <fstream>
    void test(const Scene& scene, Camera& camera) {
//        auto path = root_dir();
//        path = path / "debug" / "lines.csv";
//        std::ofstream file(path);
//        std::cout << "Writing debug csv to:\n" << path << std::endl;
        
        for (int k = 0; k < 1; k++) {
            Vec3 viewport_point;
            Ray ray = camera.make_ray(camera.screen_W / 2, camera.screen_H / 2, viewport_point);
            Hit hit;
            // auto color = ray_color(ray, max_bounces, scene);
            scene.hit(ray, Interval(camera.ray_hit_min, camera.ray_hit_max), hit);
            // print_csv({ray.origin(), viewport_point, hit.p}, file);
        }
        std::cout << std::endl;
//        file.close();
        return;
    }
    
    // older version that handles multisampling for each pixel immediatelly
    /*
    // multi-threaded: 660ms, down from 3700ms single-threaded, 5.6x speed-up
    void render(const Scene& scene, Image& image)
    {
        // return test(scene);
        
        // hardware_concurrency is 10 for M1 pro
        const int cores = std::thread::hardware_concurrency();
        
        ThreadPool thread_pool(cores, QOS_CLASS_USER_INITIATED);
        auto on_empty_callback = [&thread_pool] {
            thread_pool.stop();
        };
        thread_pool.setOnEmptyCallback(on_empty_callback);
        
        // 10 x 1 performs the best on M1 pro for 600x337 image size
        const int tile_rows = cores; // std::floor(std::sqrt(cores));
        const int tile_cols = 1;     // tile_rows;
        const int tile_height = image.H() / tile_rows;
        const int tile_width  = image.W() / tile_cols;
        int tile_id = 1;
        
        #if PRINT_PROGRESS
        printf("%d tiles\n", tile_rows*tile_cols);
        #endif
        
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
                
                int tid = tile_id++;
                thread_pool.enqueue([this, &image, &scene, y_start, twidth, theight, tid](){
                    this->render_tile(scene, image, 0, twidth, y_start, theight, tid);
                });
            }
        }
        thread_pool.join();
    }
    
    void render_tile(const Scene& scene, Image& image,
                     int x_start, int width,
                     int y_start, int height,
                     int tile_id)
    {
        for (int row = y_start; row < y_start + height; row++)
        {
            for (int col = x_start; col < x_start + width; col++)
            {
                int i = row * image.W() + col;
                Vec3& pixel = image[i];
                
                // multisampling
                for (int k = 0; k < samples_per_pixel; k++) {
                    Vec3 viewport_point;
                    Ray ray = this->make_ray(col, row, viewport_point);
                    pixel += this->ray_color(ray, max_bounces, scene);
                }
                pixel *= samples_per_pixel_inv; // average
                
                gamma_correct(pixel);
            }
        }
        #if PRINT_PROGRESS
        printf("tile %d done\n", tile_id);
        #endif
    }
     */
};

#endif /* tracer_h */
