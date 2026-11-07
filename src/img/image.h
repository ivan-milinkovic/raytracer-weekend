#ifndef Image_h
#define Image_h

#include "../math/vec3.h"
#include "color.h"
#include <vector>

struct RawImage {
    uint8_t* bytes;
    size_t w;
    size_t h;
    size_t pixel_size;
};

class Image {
private:
    int w, h;
    Vec3* pixels;
    std::vector<uint8_t> output_bytes;
    
public:
    
    Image(int width, int height)
    : w(width),
      h(height),
      output_bytes(static_cast<size_t>(width) * height * pixel_size),
      pixels(new Vec3[width * height]) {}
    
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    ~Image() {
        delete[] pixels;
    }
    
    static constexpr int pixel_size = 3;
    inline int W() const { return w; }
    inline int H() const { return h; }
    inline const Vec3 * Pixels() const { return pixels; }
    inline Vec3& operator[](int i) { return pixels[i]; }
    inline const uint8_t* OutputBytes() const { return output_bytes.data(); }
    inline RawImage raw_image() {
        return {output_bytes.data(), static_cast<size_t>(w), static_cast<size_t>(h), pixel_size};
    }
    
    void copy_for_output(double factor) {
        int j = 0;
        for (int i = 0; i < w * h; i++) {
            Vec3 p = pixels[i];
            output_bytes[j++] = (uint8_t) (p.X() * factor * 255);
            output_bytes[j++] = (uint8_t) (p.Y() * factor * 255);
            output_bytes[j++] = (uint8_t) (p.Z() * factor * 255);
        }
    }
    
    void copy_for_output_with_gamma(double factor) {
        int j = 0;
        for (int i = 0; i < w * h; i++) {
            Vec3 p = pixels[i];
            output_bytes[j++] = (uint8_t) ( linear_to_gamma(p.X() * factor ) * 255);
            output_bytes[j++] = (uint8_t) ( linear_to_gamma(p.Y() * factor ) * 255);
            output_bytes[j++] = (uint8_t) ( linear_to_gamma(p.Z() * factor ) * 255);
        }
    }
    
    void zero() {
        for (int i = 0; i < w * h; i++) {
            pixels[i] = Vec3::zero();
        }
    }
};


#endif /* Image_h */
