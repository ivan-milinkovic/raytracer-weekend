
Following: https://raytracing.github.io
  https://github.com/RayTracing/raytracing.github.io/


![book-1.png](./book-1.png)

(samples_per_pixel = 100)


![book-2.png](./book-2.png)

(samples_per_pixel = 70)

TODO: 

LambertianMaterial probably mutates std::normal_distribution when generating during render, 
which is a race condition when multithreading and sharing the same material instance.
