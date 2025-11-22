#ifndef rwmain_h
#define rwmain_h

class Image;
struct RawImage;

void rwmain(int scene_id);
Image* rw_get_image();
RawImage& rw_get_raw_image();
void rw_set_render_pass_callback(void (*render_pass_callback)(RawImage&));
void rw_set_render_progress_callback(void (*render_progress_callback)(double));

#endif // rwmain_h
