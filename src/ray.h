#ifndef Ray_h
#define Ray_h

#include "vec3.h"

class Ray {
private:
    Vec3 origin;
    Vec3 dir;
    
public:
    Ray(const Vec3& origin, const Vec3& dir): origin(origin), dir(norm(dir)) { }
    
    const Vec3& Origin() const { return origin; }
    const Vec3& Dir() const { return dir; }
    
    Vec3 at(double d) const {
        return origin + d * dir;
    }
    
};

#endif /* Ray_h */
