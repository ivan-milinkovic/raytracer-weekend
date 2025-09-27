#ifndef MYHEADER
#define MYHEADER
class Image;

void rwmain();
Image* getImage();
void setRenderPassCallback(void (*render_pass_callback)(Image*));

#endif
