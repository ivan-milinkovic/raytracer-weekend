#ifndef interval_h
#define interval_h

#include "util.h"

class Interval {
public:
    double min, max;
    
    Interval(): min(+infinity), max(-infinity) { }
    
    Interval(double min, double max) : min(min), max(max) {}
    
    Interval(const Interval& a, const Interval& b) {
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }
    
    double size() const {
        return max - min;
    }
    
    bool contains(double n) const {
        return min <= n && n <= max;
    }
    
    bool surrounds(double n) const {
        return min < n && n < max;
    }
    
    Interval expanded(double d) const {
        double d_half = d * 0.5;
        return Interval(min - d_half, max + d_half);
    }
    
    double clamp(double n) const {
        if (n < min) return min;
        if (n > max) return max;
        return n;
    }
    
    static const Interval empty, universe;
};

inline const Interval Interval::empty    = Interval(+infinity, -infinity);
inline const Interval Interval::universe = Interval(-infinity, +infinity);

#endif /* interval_h */
