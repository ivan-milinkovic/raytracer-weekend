#ifndef quad_h
#define quad_h

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"
#include "interval.h"

class Quad: public Hittable {
    public:
    Quad(const Vec3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> material)
    : Hittable(HittableType_Quad), Q(Q), u(u), v(v), material(material)
    {
        auto n = cross(u, v);
        normal = norm(n);
        D = dot(normal, Q);
        w = n / dot(n,n);
        make_bbox();
    }
    
    virtual void make_bbox() {
        auto b1 = AABB(Q, Q + v + u); // leaning right
        auto b2 = AABB(Q + u, Q + v); // leaning left (Q.x is not the minimum x)
        bbox = AABB(b1, b2);
    }
    
    AABB bounding_box() const override { return bbox; }
    
    bool hit(const Ray &ray, const Interval &limits, Hit &hit) const override {
        auto nd = dot(normal, ray.dir());
        if(std::fabs(nd) < 1e-8) return false;
        auto t = (D - dot(normal, ray.origin())) / nd;
        if (!limits.contains(t)) return false;
        
        auto p_hit = ray.at(t);
        
        Vec3 qp = p_hit - Q;
        auto uf = dot(w, cross(qp, v)); // fraction of u length
        auto vf = dot(w, cross(u, qp)); // fraction of v length
        
        auto unit_interval = Interval(0, 1);
        if (!unit_interval.contains(uf) || !unit_interval.contains(vf))
            return false;
        
        hit.d = t;
        hit.p = p_hit;
        hit.u = uf;
        hit.v = vf;
        hit.material = material;
        hit.set_normal(ray, normal);
        
        return true;
    }
    
private:
    Vec3 Q;
    Vec3 u;
    Vec3 v;
    Vec3 w;
    std::shared_ptr<Material> material;
    AABB bbox;
    Vec3 normal;
    double D;
    
};

#endif /* quad_h */
