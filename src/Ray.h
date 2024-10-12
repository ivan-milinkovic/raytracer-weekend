#ifndef Ray_h
#define Ray_h

#include "Vec3.h"

class Ray {
private:
    Vec3 origin;
    Vec3 dir;
    
public:
    Ray(const Vec3& origin, const Vec3& dir): origin(origin), dir(norm(dir)) { }
    
    const Vec3& Origin() { return origin; }
    const Vec3& Dir() { return dir; }
    
    Vec3 at(double d) {
        return origin + d * dir;
    }
    
};

#endif /* Ray_h */
