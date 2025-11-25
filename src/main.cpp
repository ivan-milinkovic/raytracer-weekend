#include "rw.h"
#include "img/ppm.h"
#include "img/image.h"

int main(int argc, const char * argv[]) {
    
    rw_init_scene(1);
    rw_render();
    
    // output
    Image* img = rw_get_image();
    write_ppm_file(img->Pixels(), img->W(), img->H());
}
