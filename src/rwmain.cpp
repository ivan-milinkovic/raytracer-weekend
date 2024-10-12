#include "ppm.h"
#include "Image.h"
#include "Ray.h"

void rwmain()
{
    Image image = Image(100, 80);
    
    for (int r = 0; r < image.H(); r++)
    {
        for (int c = 0; c < image.W(); c++)
        {
            int i = r * image.W() + c;
            Vec3& color = image[i];
            color.v[0] = r / (float) image.H();
            color.v[1] = c / (float) image.W();
            color.v[2] = (image.H() - r) / (float) image.H();
        }
    }
    print_ppm(image.Pixels(), image.W(), image.H());
}
