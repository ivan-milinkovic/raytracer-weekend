#ifndef RawImage_h
#define RawImage_h

// Used for output to Swift

struct RawImage {
    unsigned char* bytes;
    int w;
    int h;
    int pixel_size;
};

#endif /* RawImage_h */
