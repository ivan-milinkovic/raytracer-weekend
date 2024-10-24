#ifndef Vec3_apple_simd_h
#define Vec3_apple_simd_h

#ifdef VEC3_APPLE_SIMD

#include <simd/vector.h>
#include <iostream>
#include "util.h"

class Vec3
{
public:
    simd_double3 v;
//    double v[3];
    Vec3(): v{0,0,0} {}
    Vec3(simd_double3 sv): v(sv) {}
    Vec3(double x, double y, double z): v{x,y,z} {}
    
    inline double X() const { return v[0]; }
    inline double Y() const { return v[1]; }
    inline double Z() const { return v[2]; }
    
    Vec3 operator -() const {
        return Vec3(-v[0], -v[1], -v[2]);
    }
    
    inline double operator[] (int i) const { return v[i]; }
//    inline double& operator[] (int i) { return v[i]; }
    inline void set(int i, double val) { v[i] = val; }
    
    Vec3& operator+=(Vec3 v2) {
        v += v2.v;
        return *this;
    }
    
    Vec3& operator*=(double s) {
        v *= s;
        return *this;
    }
    
    double len_sq() const {
        return simd_length_squared(v);
    }
    
    double len() const {
        return simd_length(v);
    }
    
    bool is_near_zero() {
        static double e = 1e-8;
        return simd_length(simd_abs(v)) < e;
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
    return v.v + s;
}

inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
    return v1.v + v2.v;
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
    return v1.v - v2.v;
}

inline Vec3 operator*(const Vec3& v, double s) {
    return v.v * s;
}

inline Vec3 operator*(double s, const Vec3 v) {
    return v.v * s;
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
    return v1.v * v2.v;
}

inline Vec3 operator/(const Vec3& v, double s) {
    return v.v / s;
}


inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << '[' << v.v[0] << ' ' << v.v[1] << ' ' << v.v[2] << ']';
}

inline double dot(const Vec3& v1, const Vec3& v2) {
    return simd_dot(v1.v, v2.v);
}

inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
    return simd_cross(v1.v, v2.v);
}

inline Vec3 norm(const Vec3& v) {
    return simd_normalize(v.v);
}

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v.v - simd_dot(v.v, n.v) * n.v * 2;
}

inline Vec3 refract(const Vec3& v, const Vec3& n, double eta_over_eta) {
    double cos_vn = std::fmin( simd_dot(-v.v, n.v), 1.0 );
    simd_double3 r_out_perp = eta_over_eta * (v.v + (cos_vn * n.v));
    simd_double3 r_out_parallel = -std::sqrt( std::fabs(1.0 - simd_length_squared(r_out_perp))) * n.v;
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

inline static Vec3 random_unit_vector() {
    return norm(Vec3::random(-1, 1));
}

#endif

#endif /* Vec3_apple_simd_h */
