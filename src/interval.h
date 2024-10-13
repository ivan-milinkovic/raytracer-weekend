#ifndef interval_h
#define interval_h

#include "util.h"

class Interval {
public:
    double min, max;
    
    Interval(): min(+infinity), max(-infinity) { }
    
    Interval(double min, double max) : min(min), max(max) {}
    
    double size() {
        return max - min;
    }
    
    bool contains(double n) {
        return min <= n && n <= max;
    }
    
    bool surrounds(double n) {
        return min < n && n < max;
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
