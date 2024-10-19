#ifndef Vec3_h
#define Vec3_h

#include <cmath>
#include <iostream>
#include "util.h"

class Vec3
{
public:
    double v[3];
    Vec3(): v{0,0,0} {}
    Vec3(double x, double y, double z): v{x,y,z} {}
    
    inline double X() const { return v[0]; }
    inline double Y() const { return v[1]; }
    inline double Z() const { return v[2]; }
    
    Vec3 operator -() const {
        return Vec3(-v[0], -v[1], -v[2]);
    }
    
    inline double operator[] (int i) const { return v[i]; }
    inline double& operator[] (int i) { return v[i]; }
    
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
    
    bool is_near_zero() {
        static double e = 1e-8;
        return (std::fabs(v[0]) < e)
            && (std::fabs(v[1]) < e)
            && (std::fabs(v[2]) < e);
    }
    
    // [0, 1)
    inline static Vec3 random() {
        return Vec3(rw_random(), rw_random(), rw_random());
    }
    
    inline static Vec3 random(double min, double max) {
        return Vec3(rw_random(min, max), rw_random(min, max), rw_random(min, max));
    }
    
    inline static Vec3 zero() {
        return Vec3(0, 0, 0);
    }
    
    inline static Vec3 ones() {
        return Vec3(1, 1, 1);
    }
};

inline Vec3 Vec3AddScalar(const Vec3& v, double s) {
    return Vec3(v[0] + s,
                v[1] + s,
                v[2] + s);
}

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
                v1.v[1] * v2.v[1],
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

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - dot(v, n) * n * 2;
}

inline Vec3 refract(const Vec3& v, const Vec3& n, double eta_over_eta) {
    double cos_vn = std::fmin( dot(-v, n), 1.0 );
    Vec3 r_out_perp = eta_over_eta * (v + (cos_vn * n));
    Vec3 r_out_parallel = -std::sqrt( std::fabs(1.0 - r_out_perp.len_sq()) ) * n;
    return r_out_perp + r_out_parallel;
}

inline Vec3 refract(const Vec3& v, const Vec3& n, double eta_from, double eta_to) {
    double eta_over_eta = eta_from / eta_to;
    return refract(v, n, eta_over_eta);
}

inline void print_csv(const Vec3& v) {
    std::cout << v.X() << ',' << v.Y() << ',' << v.Z() << std::endl;
}

inline void print_csv(const Vec3& v0, const Vec3& v1, std::ostream& stream) {
    stream << v0.X() << ',' << v0.Y() << ',' << v0.Z() << ',';
    stream << v1.X() << ',' << v1.Y() << ',' << v1.Z() << std::endl;
}

inline void print_csv(const std::initializer_list<Vec3> vs, std::ostream& stream) {
    for(Vec3 v: vs) {
        stream << v.X() << ',' << v.Y() << ',' << v.Z() << ',';
    }
    stream << std::endl;
}

inline Vec3 random_vec3_on_hemisphere(const Vec3& normal) {
    Vec3 r = norm( Vec3::random(-1, 1) );
    if ( dot(r, normal) < 0 ) {
        r *= -1;
    }
    return r;
}

#endif /* Vec3_h */
