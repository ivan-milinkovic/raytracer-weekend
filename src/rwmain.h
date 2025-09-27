#ifndef MYHEADER
#define MYHEADER
class Image;
struct RawImage;

void rwmain();
Image* rw_get_image();
RawImage& rw_get_raw_image();
void rw_set_render_pass_callback(void (*render_pass_callback)(RawImage&));

#endif
