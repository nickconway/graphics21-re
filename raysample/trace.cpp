// ray tracer main program
// includes input file parsing and spawning screen pixel rays

// classes used directly by this file
#include "ObjectList.hpp"
#include "Polygon.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "World.hpp"
#include "Vec3.hpp"

// standard includes
#include <fstream>
#include <iostream>
#include <chrono>

int main(int argc, char **argv)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    if (argc <= 1) {
        std::cerr << "Usage: trace <file.ray>\n";
        std::cerr << "    Where <file.ray> is a rayshade scene description file\n";
        return 1;
    }

    // input file from command line or stdin
    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Error opening " << argv[1] << '\n';
        return 1;
    }

    // parse the input into everything we know about the world
    // image parameters, camera parameters
    World world(infile);

    // array of image data in ppm-file order
    unsigned char (*pixels)[3] = new unsigned char[world.height*world.width][3];

    // spawn a ray for each pixel and place the result in the pixel
    for(int j=0; j<world.height; ++j) {
        for(int i=0; i<world.width; ++i) {

            // trace new ray
            float us = world.left + (world.right  - world.left) * (i+0.5f)/world.width;
            float vs = world.top  + (world.bottom - world.top ) * (j+0.5f)/world.height;
            Vec3 dir = -world.dist * world.w + us * world.u + vs * world.v;

            Ray ray(world.eye, dir);
            Intersection isect = world.objects.trace(ray);
            Vec3 col = isect.color(world);

            // assign color
            pixels[j*world.width + i][0] = col.r();
            pixels[j*world.width + i][1] = col.g();
            pixels[j*world.width + i][2] = col.b();
        }

        if (j % 32 == 0) std::cout << "line " << j << '\n'; // render progress
    }

    // write ppm file of pixels
    std::ofstream output("trace.ppm", std::ofstream::out | std::ofstream::binary);
    output << "P6\n" << world.width << ' ' << world.height << '\n' << 255 << '\n';
    output.write((const char *)(pixels), world.height*world.width*3);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = endTime - startTime;
    std::cout << elapsed.count() << " seconds\n";

    delete[] pixels;
    return 0;
}

