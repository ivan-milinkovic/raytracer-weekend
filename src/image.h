#ifndef Image_h
#define Image_h

#include "vec3.h"
#include "color.h"

typedef struct RawImage {
    uint8_t* bytes;
    size_t w;
    size_t h;
    size_t pixel_size;
} RawImage;


class Image {
private:
    int w, h;
    Vec3* pixels;
    
public:
    Image(int width, int height) {
        w = width;
        h = height;
        pixels = new Vec3[w*h];
    }
    
    // Image(Image& other) = delete;
    
    ~Image() {
        delete pixels;
    }
    
    inline int W() const { return w; }
    inline int H() const { return h; }
    inline const Vec3 * Pixels() const { return pixels; }
    inline Vec3& operator[](int i) { return pixels[i]; }
    const int pixel_size = 3;
    
    void copy_as_pixels_to(RawImage& raw_img, double factor) {
        int j = 0;
        for (int i = 0; i < w * h; i++) {
            Vec3 p = pixels[i];
            raw_img.bytes[j++] = (uint8_t) (p.X() * factor * 255);
            raw_img.bytes[j++] = (uint8_t) (p.Y() * factor * 255);
            raw_img.bytes[j++] = (uint8_t) (p.Z() * factor * 255);
            // raw_img.bytes[j++] = (uint8_t) ( linear_to_gamma(p.X()) * factor * 255);
            // raw_img.bytes[j++] = (uint8_t) ( linear_to_gamma(p.Y()) * factor * 255);
            // raw_img.bytes[j++] = (uint8_t) ( linear_to_gamma(p.Z()) * factor * 255);
        }
    }
};



#endif /* Image_h */
