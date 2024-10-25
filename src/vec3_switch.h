#ifndef vec3_switch_h
#define vec3_switch_h

// slower
//#define VEC3_SIMD

#ifdef VEC3_SIMD
    #if defined __APPLE__
        #define VEC3_APPLE_SIMD
    #elif defined _WIN64
        #define VEC3_WIN_SIMD
    #endif
#endif

#endif /* vec3_switch_h */
