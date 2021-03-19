// draw a simple plan3 model

#include "Island.hpp"
#include "GLapp.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace glm;  // avoid glm:: for all glm types and functions

// load the island data
Island::Island(int level, vec3 size, const char *texturePPM) :
    Object(texturePPM)
{
    // build texture coordinate, normal, and vertex arrays
    uv = {
        vec2(.5, .5),
        vec2(0, .5),
        vec2(.25, .5 + .25 * sqrt(3)),
        vec2(.25, .5 + .25 * sqrt(3)),
        vec2(1, .5),
        vec2(.75, .5 - .25 * sqrt(3)),
        vec2(.75, .5 - .25 * sqrt(3))
    };

    norm = {vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f)};

    vert = {
        vec3(0, 0, size.z),
        vec3(-.5 * size.x, 0, 0),
        vec3(size.x / 2 * -.5, -.5 * size.y, 0),
        vec3(size.x / 2 * .5, -.5 * size.y, 0),
        vec3(.5 * size.x, 0, 0),
        vec3(size.x / 2 * .5, .5 * size.y, 0),
        vec3(size.x / 2 * -.5, .5 * size.y, 0),
    };

    // build index array linking sets of three vertices into triangles
    indices = {
        0, 1, 2,
        0, 2, 3,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 1
    };

    vec3 diff = cross(vert[0], vert[1]);

    // load vertex and index arrays to GPU
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), &vert[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(norm[0]), &norm[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(uv[0]), &uv[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

    updateShaders();
}
