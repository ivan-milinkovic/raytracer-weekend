#include "rwmain.h"
#include "ppm.h"
#include "image.h"

int main(int argc, const char * argv[]) {

    rwmain();
    
    // output
    Image* img = getImage();
    write_ppm_file(img->Pixels(), img->W(), img->H());
}
