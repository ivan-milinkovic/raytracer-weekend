#ifndef MYHEADER
#define MYHEADER
class Image;
struct RawImage;

void rwmain();
Image* getImage();
RawImage& getRawImage();
void setRenderPassCallback(void (*render_pass_callback)(Image*));

#endif
