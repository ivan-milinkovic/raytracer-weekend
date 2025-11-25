#ifndef rw_h
#define rw_h

class Image;
struct RawImage;

void rw_init_scene(int scene_id);
void rw_render();

void rw_set_render_pass_callback(void (*render_pass_callback)(RawImage&));
void rw_set_render_progress_callback(void (*render_progress_callback)(double));

Image* rw_get_image();
RawImage& rw_get_raw_image();

#endif // rw_h
