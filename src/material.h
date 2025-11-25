#ifndef material_h
#define material_h

#include <random>
#include "img/texture.h"

typedef enum {
    MaterialType_Lambertian,
    MaterialType_Metal,
    MaterialType_Dielectric,
    MaterialType_Diffuse,
    MaterialType_Isotropic,
} MaterialType;


class Material {
public:
    MaterialType type;
    int id = 0;
    Material(MaterialType type): type(type) { }
    
    bool visit_scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered);
    Vec3 visit_emitted(double u, double v, const Vec3& p);
};



class LambertianMaterial: public Material {
public:
    
    LambertianMaterial(const Vec3& albedo)
    : Material(MaterialType_Lambertian), tex(std::make_shared<ColorTexture>(albedo)) { }
    
    LambertianMaterial(shared_ptr<Texture> tex)
    : Material(MaterialType_Lambertian), tex(tex) { }
    
    bool scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered) {
        // Vec3 scattered_dir = norm(hit.n + Vec3::random(-1, 1));
        // Vec3 scattered_dir = random_vec3_on_hemisphere(hit.n);
        // Vec3 scattered_dir = norm(random_vec3_on_hemisphere(hit.n) + hit.n); // push it towards the normal
        Vec3 scattered_dir = norm( hit.n + Vec3 { norm_dist(gen), norm_dist(gen), norm_dist(gen) } );
        
        if (scattered_dir.is_near_zero()) {
            scattered_dir = hit.n;
        }
        Ray scattered_ray = Ray( hit.p, scattered_dir, ray.time() );
        attenuation = tex->value(hit.u, hit.v, hit.p);
        scattered = scattered_ray;
        return true;
    }
    
private:
    shared_ptr<Texture> tex;
    std::random_device random_dev {};
    std::mt19937 gen { random_dev() };
    std::normal_distribution<double> norm_dist { 0.0, 0.4 }; // https://en.wikipedia.org/wiki/Normal_distribution
    
    // True Lambertian Reflection - more rays closer to the normal
    // Imagine a unit sphere above p: centered at p + n, tangent to / touching p
    // Make a random point on the unit sphere above p: ps = p + (n + random_unit)
    // Reflecton dir: ps - p = (p + (n + rs)) - p = n + rs
    // Distribution is denser around normal, as the sphere is lifted above p
    // Since the sphere is touching p, no rays can go inside (through p), only outward
    
    // Also an option: scatter with some fixed probability p and have attenuation = albedo/p
};



class MetalMaterial: public Material {
public:
    Vec3 albedo;
    double fuzz;
    MetalMaterial(const Vec3& albedo, double fuzz):
        Material(MaterialType_Metal),
        albedo(albedo),
        fuzz( fuzz <= 1 ? fuzz : 1 ) { }
    
    bool scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered) {
        Vec3 reflected_dir = norm(reflect(ray.dir(), hit.n));
        reflected_dir = norm(reflected_dir + fuzz * Vec3::random(-1, 1));
        Ray reflected_ray = Ray( hit.p, reflected_dir, ray.time() );
        attenuation = albedo;
        scattered = reflected_ray;
        return true;
    }
};



class DielectricMaterial: public Material {
public:
    double refraction_index;
    DielectricMaterial(double refraction_index):
        Material(MaterialType_Dielectric),
        refraction_index(refraction_index) { }
    
    bool scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered) {
        attenuation = { 1, 1, 1 };
        double ri = hit.is_front ? ( 1.0 / refraction_index ) : refraction_index;
        
        Vec3 unit_dir = norm(ray.dir());
        double cos_rn = std::fmin( dot(-unit_dir, hit.n), 1.0 );
        double sin_rn = std::sqrt( 1 - cos_rn * cos_rn );
        
        Vec3 scattered_dir;
        
        bool cannot_refract = ri * sin_rn > 1.0;
        if (cannot_refract || reflectance(cos_rn, ri) > rw_random())
            scattered_dir = reflect(unit_dir, hit.n);
        else
            scattered_dir = refract(unit_dir, hit.n, ri);
        
        scattered_dir = norm(scattered_dir);
        scattered = Ray( hit.p, scattered_dir, ray.time() );
        
        return true;
    }
    
    static double reflectance(double cosine, double refraction_index) {
        // Schlick's approximation for reflectance
        double r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};


class DiffuseLightMaterial: public Material {
public:
    DiffuseLightMaterial(shared_ptr<Texture> tex) : Material(MaterialType_Diffuse), tex(tex) {}
    DiffuseLightMaterial(const Vec3& emit) : Material(MaterialType_Diffuse), tex(make_shared<ColorTexture>(emit)) {}

    bool scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered) {
        return false;
    }
    
    Vec3 emitted(double u, double v, const Vec3& p) const {
        return tex->value(u, v, p);
    }

  private:
    shared_ptr<Texture> tex;
};


class IsotropicMaterial : public Material {
  public:
    IsotropicMaterial(const Vec3& albedo)
        : Material(MaterialType_Isotropic), tex(make_shared<ColorTexture>(albedo)) { }
    
    IsotropicMaterial(shared_ptr<Texture> tex)
        : Material(MaterialType_Isotropic), tex(tex) { }

    bool scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered)
    {
        scattered = Ray(hit.p, random_unit_vector(), ray.time());
        attenuation = tex->value(hit.u, hit.v, hit.p);
        return true;
    }

  private:
    shared_ptr<Texture> tex;
};


bool Material::visit_scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered) {
    // Using static dispatch instead of inheritance vtable dynamic dispatch
    switch (type) {
        case MaterialType_Lambertian: {
            LambertianMaterial* lm = static_cast<LambertianMaterial*>(this);
            return lm->scatter(ray, hit, attenuation, scattered);
        }
        case MaterialType_Metal: {
            MetalMaterial* mm = static_cast<MetalMaterial*>(this);
            return mm->scatter(ray, hit, attenuation, scattered);
        }
        case MaterialType_Dielectric: {
            DielectricMaterial* dm = static_cast<DielectricMaterial*>(this);
            return dm->scatter(ray, hit, attenuation, scattered);
        }
        case MaterialType_Diffuse: {
            DiffuseLightMaterial* dm = static_cast<DiffuseLightMaterial*>(this);
            return dm->scatter(ray, hit, attenuation, scattered);
        }
        case MaterialType_Isotropic: {
            IsotropicMaterial* im = static_cast<IsotropicMaterial*>(this);
            return im->scatter(ray, hit, attenuation, scattered);
        }
        default: {
            std::cout << "Unhandled material in: " << __FUNCTION__ << std::endl;
            exit(1);
        }
    }
    return false;
}

Vec3 Material::visit_emitted(double u, double v, const Vec3& p) {
    // Using static dispatch instead of inheritance vtable dynamic dispatch
    switch (type) {
        case MaterialType_Diffuse: {
            DiffuseLightMaterial* dm = static_cast<DiffuseLightMaterial*>(this);
            return dm->emitted(u, v, p);
        }
        default: {
            return Vec3::zero();
        }
    }
}

#endif /* material_h */
