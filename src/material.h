#ifndef material_h
#define material_h

typedef enum {
    MaterialType_Lambertian,
    MaterialType_Metal,
    MaterialType_Dielectric,
} MaterialType;


class Material {
public:
    MaterialType type;
    int id = 0;
    Material(MaterialType type): type(type) { }
    
    bool visit_scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered);
};



class LambertianMaterial: public Material {
public:
    Vec3 albedo;
    LambertianMaterial(const Vec3& albedo): Material(MaterialType_Lambertian), albedo(albedo) { }
    
    bool scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered) {
        Vec3 scattered_dir = norm(hit.n + Vec3::random(-1, 1));
        if (scattered_dir.is_near_zero()) {
            scattered_dir = hit.n;
        }
        Ray scattered_ray = Ray( hit.p, scattered_dir, ray.time() );
        attenuation = albedo;
        scattered = scattered_ray;
        return true;
    }
    
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



bool Material::visit_scatter(const Ray& ray, const Hit& hit, Vec3& attenuation, Ray& scattered) {
    // Using static dispatch instead of inheritance vtable dynamic dispatch
    switch (type) {
        case MaterialType_Lambertian: {
            LambertianMaterial* lm = static_cast<LambertianMaterial*>(this);
            lm->scatter(ray, hit, attenuation, scattered);
            return true;
        }
        case MaterialType_Metal: {
            MetalMaterial* mm = static_cast<MetalMaterial*>(this);
            mm->scatter(ray, hit, attenuation, scattered);
            return true;
        }
        case MaterialType_Dielectric: {
            DielectricMaterial* dm = static_cast<DielectricMaterial*>(this);
            dm->scatter(ray, hit, attenuation, scattered);
            return true;
        }
        default: {
            std::cout << "Unhandled material in: " << __PRETTY_FUNCTION__ << std::endl;
            exit(1);
        }
    }
    return false;
}

#endif /* material_h */
