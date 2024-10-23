#ifndef constant_medium_h
#define constant_medium_h

#include "hittable.h"
#include "material.h"
#include "texture.h"

class ConstantMedium : public Hittable {
  public:
    ConstantMedium(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> tex)
      : Hittable(HittableType_ConstantMedium), boundary(boundary), neg_inv_density(-1/density),
        phase_function(make_shared<IsotropicMaterial>(tex))
    {}

    ConstantMedium(shared_ptr<Hittable> boundary, double density, const Vec3& albedo)
      : Hittable(HittableType_ConstantMedium),
        boundary(boundary), neg_inv_density(-1/density),
        phase_function(make_shared<IsotropicMaterial>(albedo))
    {}

    bool hit(const Ray& r, const Interval& limits, Hit& hit) const override {
        Hit hit1, hit2;

        if (!boundary->hit(r, Interval::universe, hit1))
            return false;

        if (!boundary->hit(r, Interval(hit1.d+0.0001, infinity), hit2))
            return false;

        if (hit1.d < limits.min) hit1.d = limits.min;
        if (hit2.d > limits.max) hit2.d = limits.max;

        if (hit1.d >= hit2.d)
            return false;

        if (hit1.d < 0)
            hit1.d = 0;

        auto ray_length = r.dir().len();
        auto distance_inside_boundary = (hit2.d - hit1.d) * ray_length;
        auto hit_distance = neg_inv_density * std::log(rw_random());

        if (hit_distance > distance_inside_boundary)
            return false;

        hit.d = hit1.d + hit_distance / ray_length;
        hit.p = r.at(hit.d);

        hit.n = Vec3(1,0,0);  // arbitrary
        hit.is_front = true;  // also arbitrary
        hit.material = phase_function;

        return true;
    }

    AABB bounding_box() const override { return boundary->bounding_box(); }

  private:
    shared_ptr<Hittable> boundary;
    double neg_inv_density;
    shared_ptr<Material> phase_function;
};


#endif /* constant_medium_h */
