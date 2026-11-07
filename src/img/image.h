#ifndef Image_h
#define Image_h

#include "../math/vec3.h"
#include "raw_image.h"
#include "color.h"
#include <vector>


class Image {
private:
    int w, h;
    Vec3* pixels;
    std::vector<uint8_t> output_bytes;
    
public:
    
    Image(int width, int height)
    : w(width),
      h(height),
      output_bytes(static_cast<std::size_t>(width) * height * pixel_size),
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
        return {reinterpret_cast<unsigned char*>(output_bytes.data()), w, h, pixel_size};
    }
    
    void copy_for_output(double factor) {
        int j = 0;
        for (int i = 0; i < w * h; i++) {
            Vec3 p = pixels[i];
            output_bytes[j++] = output_byte(p.X() * factor, false);
            output_bytes[j++] = output_byte(p.Y() * factor, false);
            output_bytes[j++] = output_byte(p.Z() * factor, false);
        }
    }
    
    void copy_for_output_with_gamma(double factor) {
        int j = 0;
        for (int i = 0; i < w * h; i++) {
            Vec3 p = pixels[i];
            output_bytes[j++] = output_byte(p.X() * factor, true);
            output_bytes[j++] = output_byte(p.Y() * factor, true);
            output_bytes[j++] = output_byte(p.Z() * factor, true);
        }
    }
    
    void zero() {
        for (int i = 0; i < w * h; i++) {
            pixels[i] = Vec3::zero();
        }
    }
    
private:
    
    static uint8_t output_byte(double value, bool apply_gamma) {
        if (!std::isfinite(value))
            return 0;
        if (apply_gamma)
            value = linear_to_gamma(value);
        return static_cast<uint8_t>(std::clamp(value, 0.0, 1.0) * 255);
    }
    
};


#endif /* Image_h */
