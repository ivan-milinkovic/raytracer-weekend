#ifndef Ray_h
#define Ray_h

#include "vec3.h"

class Ray {
private:
    Vec3 _origin;
    Vec3 _dir;
    double _time;
    
public:
    Ray(const Vec3& origin, const Vec3& dir): _origin(origin), _dir(norm(dir)) { }
    Ray(const Vec3& origin, const Vec3& dir, double time): _origin(origin), _dir(norm(dir)), _time(time) { }
    Ray() { }
    
    const Vec3& origin() const { return _origin; }
    const Vec3& dir() const { return _dir; }
    double time() const { return _time; }
    
    Vec3 at(double d) const {
        return _origin + _dir * d;
    }    
};

#endif /* Ray_h */
