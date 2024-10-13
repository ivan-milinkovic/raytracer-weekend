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
    
    static const Interval empty, universe;
};

const Interval Interval::empty    = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, +infinity);

#endif /* interval_h */
