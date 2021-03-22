// draw a simple plan3 model

#include "Island.hpp"
#include "GLapp.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace glm;  // avoid glm:: for all glm types and functions

std::vector<vec2> Island::calculateNewUV() {

    sharedVertices.clear();

    std::vector<vec2> newUV = uv;
    for (int i = 0; i < indices.size(); i += 3) {

        std::vector<vec2> triangle = { uv[indices[i]], uv[indices[i + 1]], uv[indices[i + 2]] };

        vec2 newVertex1(0, 0);
        vec2 newVertex2(0, 0);
        vec2 newVertex3(0, 0);

        newVertex1.x = (triangle[0].x + triangle[1].x) / 2;
        newVertex1.y = (triangle[0].y + triangle[1].y) / 2;
        auto it = newUV.begin();
        for (int i = 0; i < newUV.size(); i++) {
            if (*it == newVertex1) {
                sharedVertices.emplace(newUV.size(), i);
            }

            it++;
        }
        newUV.push_back(newVertex1);
        perimeter.push_back(perimeter[indices[i]] && perimeter[indices[i + 1]]);

        newVertex2.x = (triangle[1].x + triangle[2].x) / 2;
        newVertex2.y = (triangle[1].y + triangle[2].y) / 2;
        it = newUV.begin();
        for (int i = 0; i < newUV.size(); i++) {
            if (*it == newVertex2) {
                sharedVertices.emplace(newUV.size(), i);
            }

            it++;
        }
        newUV.push_back(newVertex2);
        perimeter.push_back(perimeter[indices[i + 1]] && perimeter[indices[i + 2]]);

        newVertex3.x = (triangle[2].x + triangle[0].x) / 2;
        newVertex3.y = (triangle[2].y + triangle[0].y) / 2;
        it = newUV.begin();
        for (int i = 0; i < newUV.size(); i++) {
            if (*it == newVertex3) {
                sharedVertices.emplace(newUV.size(), i);
            }

            it++;
        }
        newUV.push_back(newVertex3);
        perimeter.push_back(perimeter[indices[i + 2]] && perimeter[indices[i]]);

    }

    return newUV;

}

std::vector<vec3> Island::calculateNewVert() {

    std::srand(time(NULL));

    std::vector<vec3> newVert = vert;
    for (int i = 0; i < indices.size(); i += 3) {

        double zOffset = ((double)rand() / RAND_MAX * 2.0 - 1.0) * (pow(2, -level) * vert[0][2]);

        std::vector<vec3> triangle = { vert[indices[i]], vert[indices[i + 1]], vert[indices[i + 2]] };

        vec3 newVertex1(0, 0, 0);
        vec3 newVertex2(0, 0, 0);
        vec3 newVertex3(0, 0, 0);

        newVertex1.x = (triangle[0].x + triangle[1].x) / 2;
        newVertex1.y = (triangle[0].y + triangle[1].y) / 2;
        newVertex1.z = (triangle[0].z + triangle[1].z) / 2 + zOffset;
        if (newVertex1.z > 0 && perimeter[newVert.size()]) {
            newVertex1.z = 0;
        }
        if (sharedVertices[newVert.size()]) {
            newVertex1 = newVert[sharedVertices[newVert.size()]];
        }
        newVert.push_back(newVertex1);

        newVertex2.x = (triangle[1].x + triangle[2].x) / 2;
        newVertex2.y = (triangle[1].y + triangle[2].y) / 2;
        newVertex2.z = (triangle[1].z + triangle[2].z) / 2 + zOffset;
        if (newVertex2.z > 0 && perimeter[newVert.size()]) {
            newVertex2.z = 0;
        }
        if (sharedVertices[newVert.size()]) {
            newVertex2 = newVert[sharedVertices[newVert.size()]];
        }
        newVert.push_back(newVertex2);

        newVertex3.x = (triangle[2].x + triangle[0].x) / 2;
        newVertex3.y = (triangle[2].y + triangle[0].y) / 2;
        newVertex3.z = (triangle[2].z + triangle[0].z) / 2 + zOffset;
        if (newVertex3.z > 0 && perimeter[newVert.size()]) {
            newVertex3.z = 0;
        }
        if (sharedVertices[newVert.size()]) {
            newVertex3 = newVert[sharedVertices[newVert.size()]];
        }
        newVert.push_back(newVertex3);

    }

    return newVert;

}

std::vector<unsigned int> Island::calculateNewIndices() {

    std::vector<unsigned int> newIndices;

    for (int i = 0; i < indices.size(); i+=3) {

        unsigned int newVertIndex = i + levelVert[level - 1].size();

        newIndices.push_back(indices[i]);
        newIndices.push_back(newVertIndex);
        newIndices.push_back(newVertIndex + 2);

        newIndices.push_back(indices[i + 1]);
        newIndices.push_back(newVertIndex + 1);
        newIndices.push_back(newVertIndex);

        newIndices.push_back(indices[i + 2]);
        newIndices.push_back(newVertIndex + 2);
        newIndices.push_back(newVertIndex + 1);

        newIndices.push_back(newVertIndex);
        newIndices.push_back(newVertIndex + 1);
        newIndices.push_back(newVertIndex + 2);

    }

    return newIndices;

}

std::vector<vec3> Island::calculateNorms() {

    std::vector<vec3> newNorm;
    for (auto vertex : vert) {

        vec3 sum(0, 0, 0);

        for (int i = 0; i < indices.size(); i += 3) {
            std::vector<vec3> triangle = { vert[indices[i]], vert[indices[i + 1]], vert[indices[i + 2]] };
            for (auto v : triangle) {

                if (vertex == v) {

                    sum += cross(triangle[1] - triangle[0], triangle[2] - triangle[0]);

                }
            }

        }

        newNorm.push_back(normalize(sum));
    }

    return newNorm;

}

// load the island data
Island::Island(vec3 size, const char *texturePPM) :
    Object(texturePPM)
{

    level = 0;

    // build texture coordinate, normal, and vertex arrays
    uv = {
        vec2(.5, .5),
        vec2(0, .5),
        vec2(.25, .5 + .25 * sqrt(3)),
        vec2(.75, .5 + .25 * sqrt(3)),
        vec2(1, .5),
        vec2(.75, .5 - .25 * sqrt(3)),
        vec2(.25, .5 - .25 * sqrt(3))
    };

    vert = {
        vec3(0, 0, size.z),
        vec3(-.5 * size.x, 0, 0),
        vec3(size.x / 2 * -.5, -.5 * size.y, 0),
        vec3(size.x / 2 * .5, -.5 * size.y, 0),
        vec3(.5 * size.x, 0, 0),
        vec3(size.x / 2 * .5, .5 * size.y, 0),
        vec3(size.x / 2 * -.5, .5 * size.y, 0),
    };

    perimeter = { 0, 1, 1, 1, 1, 1, 1 };

    // build index array linking sets of three vertices into triangles
    indices = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 1,
    };

    norm = calculateNorms();

    levelUV.push_back(uv);
    levelVert.push_back(vert);
    levelIndices.push_back(indices);
    levelNorm.push_back(norm);

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


void Island::addSubdivision() {

    level++;

    if (levelUV.size() > level) {

        uv = levelUV[level];
        vert = levelVert[level];
        indices = levelIndices[level];
        norm = levelNorm[level];

    }
    else {

        uv = calculateNewUV();
        vert = calculateNewVert();
        indices = calculateNewIndices();
        norm = calculateNorms();

        levelUV.push_back(uv);
        levelVert.push_back(vert);
        levelIndices.push_back(indices);
        levelNorm.push_back(norm);

    }

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

void Island::removeSubdivision() {

    level--;

    uv = levelUV[level];
    vert = levelVert[level];
    indices = levelIndices[level];
    norm = levelNorm[level];

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