// Everything we know about the world
#ifndef World_hpp
#define World_hpp

// other classes we use DIRECTLY in our interface
#include "Vec3.hpp"
#include "ObjectList.hpp"
#include <fstream>

class World {
public: // public data

    // image size
    int width, height;

    // background color
    Vec3 background;

    // view origin and basis parameters
    Vec3 eye, w, u, v;
    float dist, left, right, bottom, top;


    // list of objects in the scene
    ObjectList objects;

public:                                 
    // read world data from a file
    World(std::istream &ifile); 
};

#endif
