#ifndef Image_h
#define Image_h

#include "Vec3.h"

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
    
    ~Image() {
        delete pixels;
    }
    
    inline int W() const { return w; }
    inline int H() const { return h; }
    inline const Vec3 * Pixels() { return pixels; }
    inline Vec3& operator[](int i) { return pixels[i]; }
};

#endif /* Image_h */
