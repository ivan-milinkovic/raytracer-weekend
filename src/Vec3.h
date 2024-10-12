#ifndef Vec3_h
#define Vec3_h

#include <cmath>
#include <iostream>

class Vec3
{
public:
    double v[3];
    Vec3(): v{0,0,0} {}
    Vec3(double x, double y, double z): v{x,y,z} {}
    
    double x() const { return v[0]; }
    double y() const { return v[1]; }
    double z() const { return v[2]; }
    
    Vec3 operator -() const {
        return Vec3(-v[0], -v[1], -v[2]);
    }
    
    double operator[](int i) const { return v[i]; }
    double& operator[](int i) { return v[i]; }
    
    Vec3& operator+=(Vec3 v2) {
        v[0] += v2[0];
        v[1] += v2[1];
        v[2] += v2[2];
        return *this;
    }
    
    Vec3& operator*=(double s) {
        v[0] *= s;
        v[1] *= s;
        v[2] *= s;
        return *this;
    }
    
    double len_sq() const {
        return v[0] * v[0]
             + v[1] * v[1]
             + v[2] * v[2];
    }
    
    double len() const {
        return std::sqrt(len_sq());
    }
};

inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1[0] + v2[0],
                v1[1] + v2[1],
                v1[2] + v2[2]);
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1[0] - v2[0],
                v1[1] - v2[1],
                v1[2] - v2[2]);
}

inline Vec3 operator*(const Vec3& v, double s) {
    return Vec3(v[0] * s,
                v[1] * s,
                v[2] * s);
}

inline Vec3 operator*(double s, const Vec3 v) {
    return Vec3(v[0] * s,
                v[1] * s,
                v[2] * s);
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1.v[0] * v2.v[0],
                v2.v[1] * v2.v[1],
                v1.v[2] * v2.v[2]);
}

inline Vec3 operator/(const Vec3& v, double s) {
    return v * (1/s);
}


inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << '[' << v.v[0] << ' ' << v.v[1] << ' ' << v.v[2] << ']';
}

inline double dot(const Vec3& v1, const Vec3& v2) {
    return v1.v[0] * v2.v[0]
         + v1.v[1] * v2.v[1]
         + v1.v[2] * v2.v[2];
}

inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1],
                v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2],
                v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0]);
}

inline Vec3 norm(const Vec3& v) {
    return v / v.len();
}

#endif /* Vec3_h */
