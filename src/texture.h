#ifndef texture_h
#define texture_h


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

#endif /* texture_h */
