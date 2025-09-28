#include "rwmain.h"
#include "ppm.h"
#include "image.h"
// #include "thread_pool.h"

int main(int argc, const char * argv[]) {
    
    // ThreadPool::test(); return 0;
    
    rwmain(1);
    
    // output
    Image* img = rw_get_image();
    write_ppm_file(img->Pixels(), img->W(), img->H());
}
