// data and functions to draw a plane
#pragma once

#include "Object.hpp"
#include <map>

// plane object
class Island : public Object {
public:
    int level;
    std::vector<bool> perimeter;
    std::map<unsigned int, unsigned int> sharedVertices;
   
    std::vector<glm::vec2> calculateNewUV();
    std::vector<glm::vec3> calculateNewVert();
    std::vector<unsigned int> calculateNewIndices();
    std::vector<glm::vec3> calculateNorms();

    std::vector<std::vector<glm::vec3>> levelVert;        //   per-vertex position
    std::vector<std::vector<glm::vec3>> levelNorm;        //   per-vertex normal
    std::vector<std::vector<glm::vec2>> levelUV;          //   per-vertex texture coordinate
    std::vector<std::vector<unsigned int>> levelIndices;

    // create plane from -size/2 to size/2
    Island(glm::vec3 size, const char *texturePPM);

    void addSubdivision();
    void removeSubdivision();
    void reset();
};
