#ifndef RawImage_h
#define RawImage_h

#include <stddef.h>
#include <stdint.h>

// Used for output to Swift

struct RawImage {
    uint8_t* bytes;
    size_t w;
    size_t h;
    size_t pixel_size;
};

#endif /* RawImage_h */
