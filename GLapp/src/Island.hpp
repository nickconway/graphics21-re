// data and functions to draw a plane
#pragma once

#include "Object.hpp"

// plane object
class Island : public Object {
public:
    // create plane from -size/2 to size/2
    Island(int level, glm::vec3 size, const char *texturePPM);
};
