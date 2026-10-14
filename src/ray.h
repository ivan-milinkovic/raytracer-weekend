#ifndef Ray_h
#define Ray_h

#include "math/vec3.h"

class Ray {
private:
    Vec3 _origin;
    Vec3 _dir;
    double _time;
    Vec3 _one_over_dir;
    
public:
    Ray(const Vec3& origin, const Vec3& dir):
        _origin(origin),
        _dir(norm(dir)),
        _one_over_dir(1.0 / _dir.X(), 1.0 / _dir.Y(), 1.0 / _dir.Z())
    { }
    Ray(const Vec3& origin, const Vec3& dir, double time):
        _origin(origin),
        _dir(norm(dir)),
        _time(time),
        _one_over_dir(1.0 / _dir.X(), 1.0 / _dir.Y(), 1.0 / _dir.Z())
    { }
    Ray() { }
    
    const Vec3& origin() const { return _origin; }
    const Vec3& dir() const { return _dir; }
    const Vec3& one_over_dir() const { return _one_over_dir; }
    double time() const { return _time; }
    
    Vec3 at(double d) const {
        return _origin + _dir * d;
    }    
};

#endif /* Ray_h */
