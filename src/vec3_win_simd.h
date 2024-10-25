#ifndef Vec3_win_simd_h
#define Vec3_win_simd_h

#ifdef VEC3_WIN_SIMD

// https://gist.github.com/bangonkali/52137c168f9bb0aaf003

#include <iostream>
#include "util.h"
#include <intrin.h>

class Vec3
{
public:
    __m128 v;
    Vec3(): v{0,0,0} {}
    Vec3(__m128 sv): v(sv) {}
    Vec3(double x, double y, double z) : v{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) } {}
    
    inline double X() const { return v.m128_f32[0]; }
    inline double Y() const { return v.m128_f32[1]; }
    inline double Z() const { return v.m128_f32[2]; }
    
    Vec3 operator -() const {
        return Vec3(-v.m128_f32[0], -v.m128_f32[1], -v.m128_f32[2]);
    }
    
    inline double operator[] (int i) const { return v.m128_f32[i]; }
//    inline double& operator[] (int i) { return v[i]; }
    inline void set(int i, double val) { v.m128_f32[i] = static_cast<float>(val); }
    
    Vec3& operator+=(Vec3 v2) {
        v = _mm_add_ps(v, v2.v);
        return *this;
    }
    
    Vec3& operator*=(double s) {
        v = _mm_mul_ps(v, _mm_set1_ps(static_cast<float>(s)));
        return *this;
    }
    
    double len_sq() const {
        auto v2 = _mm_mul_ps(v, v);
        return v2.m128_f32[0] + v2.m128_f32[1] + v2.m128_f32[2];
    }
    
    double len() const {
        return std::sqrt(len_sq());
    }
    
    bool is_near_zero() {
        static double e = 1e-8;
        return fabs(v.m128_f32[0]) < e && fabs(v.m128_f32[1]) < e && fabs(v.m128_f32[2]) < e;
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
    return _mm_add_ps(v.v, _mm_set1_ps(static_cast<float>(s)));
}

inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
    return _mm_add_ps(v1.v, v2.v);
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
    return _mm_sub_ps(v1.v, v2.v);
}

inline Vec3 operator*(const Vec3& v, double s) {
    return _mm_mul_ps(v.v, _mm_set1_ps(static_cast<float>(s)));
}

inline Vec3 operator*(double s, const Vec3 v) {
    return _mm_mul_ps(v.v, _mm_set1_ps(static_cast<float>(s)));
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
    return _mm_mul_ps(v1.v, v2.v);
}

inline Vec3 operator/(const Vec3& v, double s) {
    return _mm_mul_ps(v.v, _mm_set1_ps(static_cast<float>(1.0/s)));
}


inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << '[' << v.v.m128_f32[0] << ' ' << v.v.m128_f32[1] << ' ' << v.v.m128_f32[2] << ']';
}

inline double dot(const Vec3& v1, const Vec3& v2) {
    return v1.X() * v2.X()
         + v1.Y() * v2.Y()
         + v1.Z() * v2.Z();
}

inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1.Y() * v2.Z() - v1.Z() * v2.Y(),
                v1.Z() * v2.X() - v1.X() * v2.Z(),
                v1.X() * v2.Y() - v1.Y() * v2.X());
}

inline Vec3 norm(const Vec3& v) {
    auto len = v.len();
    return Vec3(v.v / len);
}

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v.v - dot(v, n) * n.v * 2;
}

inline Vec3 refract(const Vec3& v, const Vec3& n, double eta_over_eta) {
    double cos_vn = std::fmin( dot(-v, n), 1.0 );
    auto r_out_perp = eta_over_eta * (v + (cos_vn * n));
    auto r_out_parallel = -std::sqrt( std::fabs(1.0 - r_out_perp.len_sq())) * n;
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

#endif /* Vec3_win_simd_h */
