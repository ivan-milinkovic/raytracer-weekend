#ifndef texture_h
#define texture_h

#include "perlin.h"

class Texture {
public:
    virtual Vec3 value(double u, double v, const Vec3& p) const = 0;
    virtual ~Texture() = default;
};


class ColorTexture: public Texture {
public:
    
    ColorTexture(const Vec3& color): albedo(color) { }
    ColorTexture(double r, double g, double b): albedo(Vec3(r,g,b)) { }
    
    Vec3 value(double u, double v, const Vec3& p) const override {
        return albedo;
    }
    
private:
    Vec3 albedo;
};


class CheckerTexture: public Texture {
public:
    CheckerTexture(double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
    : one_over_scale(1.0/scale), even(even), odd(odd) { }
    
    CheckerTexture(double scale, Vec3 even_color, Vec3 odd_color)
    : CheckerTexture(scale, std::make_shared<ColorTexture>(even_color),
                            std::make_shared<ColorTexture>(odd_color))
    { }
    
    Vec3 value(double u, double v, const Vec3 &p) const override {
        auto xi = int( std::floor(p.X() * one_over_scale) );
        auto yi = int( std::floor(p.Y() * one_over_scale) );
        auto zi = int( std::floor(p.Z() * one_over_scale) );
        
        bool is_even = (xi + yi + zi) % 2 == 0;
        
        return is_even ? even->value(u, v, p) : odd->value(u, v, p) ;
    }
    
private:
    double one_over_scale;
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};


#include "rwimage.h"

class ImageTexture: public Texture {
public:
    ImageTexture(const char* file_path): image(file_path) { }
    
    Vec3 value(double u, double v, const Vec3 &p) const override {
        if (image.height() <= 0) return Vec3(0,1,1);
        u = 1.0 - Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);
        
        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i,j);
        
        auto color_scale = 1.0 / 255.0;
        return Vec3(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }
    
private:
    RwImage image;
};


class PerlinTexture: public Texture {
public:
    PerlinTexture(double scale): scale(scale) { }
    
    Vec3 value(double u, double v, const Vec3 &p) const override {
        return Vec3(.5, .5, .5) * (1 + std::sin(scale * p.Z() + 10 * noise.turb(p, 7)));
    }
    
private:
    Perlin noise;
    double scale;
};


#endif /* texture_h */
