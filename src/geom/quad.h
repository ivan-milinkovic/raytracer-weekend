#ifndef quad_h
#define quad_h

#include "hittable.h"
#include "hittable_list.h"
#include "aabb.h"
#include "../math/vec3.h"
#include "../math/interval.h"

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
        auto b1 = AABB(Q, Q + u + v); // leaning right
        auto b2 = AABB(Q + u, Q + v); // leaning left (Q.x is not the minimum x)
        bbox = AABB(b1, b2);
    }
    
    AABB bounding_box() const { return bbox; }
    
    bool hit(const Ray &ray, const Interval &limits, Hit &hit) const {
        auto nd = dot(normal, ray.dir());
        if(std::fabs(nd) < 1e-8) return false;
        auto t = (D - dot(normal, ray.origin())) / nd;
        if (!limits.contains(t)) return false;
        
        auto p_hit = ray.at(t);
        Vec3 qp = p_hit - Q;
        auto a = dot(w, cross(qp, v)); // fraction of u length
        auto b = dot(w, cross(u, qp)); // fraction of v length
        if(!is_interior(a, b)) return false;
        
        hit.d = t;
        hit.p = p_hit;
        hit.u = a;
        hit.v = b;
        hit.material = material.get();
        hit.set_normal(ray, normal);
        
        return true;
    }
    
    virtual inline bool is_interior(double a, double b) const {
        auto unit_interval = Interval(0, 1);
        return unit_interval.contains(a) && unit_interval.contains(b);
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


inline
vector<shared_ptr<Hittable>>
make_box(const Vec3& a, const Vec3& b, shared_ptr<Material> mat)
{
    vector<shared_ptr<Hittable>> sides;
    
    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = Vec3(std::fmin(a.X(),b.X()), std::fmin(a.Y(),b.Y()), std::fmin(a.Z(),b.Z()));
    auto max = Vec3(std::fmax(a.X(),b.X()), std::fmax(a.Y(),b.Y()), std::fmax(a.Z(),b.Z()));
    
    auto dx = Vec3(max.X() - min.X(), 0, 0);
    auto dy = Vec3(0, max.Y() - min.Y(), 0);
    auto dz = Vec3(0, 0, max.Z() - min.Z());

    sides.push_back(make_shared<Quad>( Vec3(min.X(), min.Y(), max.Z()),  dx,  dy, mat)); // front
    sides.push_back(make_shared<Quad>( Vec3(max.X(), min.Y(), max.Z()), -dz,  dy, mat)); // right
    sides.push_back(make_shared<Quad>( Vec3(max.X(), min.Y(), min.Z()), -dx,  dy, mat)); // back
    sides.push_back(make_shared<Quad>( Vec3(min.X(), min.Y(), min.Z()),  dz,  dy, mat)); // left
    sides.push_back(make_shared<Quad>( Vec3(min.X(), max.Y(), max.Z()),  dx, -dz, mat)); // top
    sides.push_back(make_shared<Quad>( Vec3(min.X(), min.Y(), min.Z()),  dx,  dz, mat)); // bottom

    return sides;
}



inline
shared_ptr<HittableList>
make_box_2(const Vec3& a, const Vec3& b, shared_ptr<Material> mat)
{
    return make_shared<HittableList>(make_box(a, b, mat));
}


class Triangle: public Quad {
public:
    Triangle(const Vec3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> material)
    : Quad(Q, u, v, material) { }
    
    inline bool is_interior(double a, double b) const override {
        return a > 0 && b > 0 && a+b<1;
    }
};


class Disk: public Quad {
public:
    Disk(double r, const Vec3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> material)
    : Quad(Q, u, v, material), r(r) { }
    
    inline bool is_interior(double a, double b) const override {
        return a*a + b*b <= r*r;
    }
    
private:
    double r;
};

#endif /* quad_h */
