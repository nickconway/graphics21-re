// draw a player model

#include "Player.hpp"
#include "GLapp.hpp"
#include <math.h>
#include "Island.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdlib.h>

using namespace glm;  // avoid glm:: for all glm types and functions

#ifndef F_PI
#define F_PI 3.1415926f
#endif

float zOffset = 5;

// load the sphere data
Player::Player(int w, int h, vec3 size, const char* texturePPM, Island* i) : Object(texturePPM){

    xPos = yPos = zPos = 0;
    island = i;

    // build vertex, normal and texture coordinate arrays
    // * x & y are longitude and latitude grid positions
    for (unsigned int y = 0; y <= h; ++y) {
        for (unsigned int x = 0; x <= w; ++x) {
            // Texture coordinates scaled from x and y. Be sure to cast before division!
            float u = float(x) / float(w), v = float(y) / float(h);
            uv.push_back(vec2(u, v));

            // normal for sphere is normalized position in spherica coordinates
            float cx = cosf(2.f * F_PI * u), sx = sinf(2.f * F_PI * u);
            float cy = cosf(F_PI * v), sy = sinf(F_PI * v);
            vec3 N = vec3(cx * sy, sx * sy, cy);
            norm.push_back(N);

            // 3d vertex location scaled by sphere size
            vert.push_back(size * N);
        }
    }

    // build index array linking sets of three vertices into triangles
    // two triangles per square in the grid. Each vertex index is
    // essentially its unfolded grid array position. Be careful that
    // each triangle ends up in counter-clockwise order
    for (unsigned int y = 0; y < h; ++y) {
        for (unsigned int x = 0; x < w; ++x) {
            indices.push_back((w + 1) * y + x);
            indices.push_back((w + 1) * y + x + 1);
            indices.push_back((w + 1) * (y + 1) + x + 1);

            indices.push_back((w + 1) * y + x);
            indices.push_back((w + 1) * (y + 1) + x + 1);
            indices.push_back((w + 1) * (y + 1) + x);
        }
    }

    // load vertex and index array to GPU
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

// Get barycentric coordinates for a triangle from the player position
std::vector<float> Player::barycentric(std::vector<vec3> triangle) {

    float alpha, beta, gamma;
    vec3 v0(triangle[0].x, triangle[0].y, 0);
    vec3 v1(triangle[1].x, triangle[1].y, 0);
    vec3 v2(triangle[2].x, triangle[2].y, 0);
    vec3 p(xPos, yPos, 0);

    vec3 n = cross((v2 - v1), (v0 - v1));

    vec3 nAlpha = cross((v2 - v1), (p - v1));
    vec3 nBeta = cross((v1 - v0), (p - v0));
    vec3 nGamma = cross((v0 - v2), (p - v2));

    alpha = dot(n, nAlpha) / dot(n, n);
    beta = dot(n, nBeta) / dot(n, n);
    gamma = 1 - alpha - beta;

    return { alpha, beta, gamma };

}

// Move the player
void Player::move(GLapp* app) {

    float xWSMultiplier = sin(app->pan);
    float yWSMultiplier = cos(app->pan);

    float xADMultiplier = cos(-app->pan);
    float yADMultiplier = sin(-app->pan);

    float xComponent = (forwardSpeed * xWSMultiplier) + (strafeSpeed * xADMultiplier);
    float yComponent = (forwardSpeed * yWSMultiplier) + (strafeSpeed * yADMultiplier);

    xPos += xComponent;
    yPos += yComponent;
    zPos = zOffset;

    float alpha = 0, beta = 0, gamma = 0;

    // Go through each triangle
    for (int i = 0; i < island->indices.size(); i += 3) {
        std::vector<vec3> triangle = { island->vert[island->indices[i]], island->vert[island->indices[i + 1]], island->vert[island->indices[i + 2]] };

        std::vector<float> baryCoords = barycentric(triangle);

        alpha = baryCoords[0];
        beta = baryCoords[1];
        gamma = baryCoords[2];

        // Only update when the coordinates are valid
        if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1) {
            zPos = zOffset + triangle[0].z * alpha + triangle[1].z * beta + triangle[2].z * gamma;
        }
    }

    // Keep from going below sea level
    zPos = max(zPos, zOffset);

}

//
// this is called every time the sphere needs to be redrawn 
//
void Player::draw(GLapp* app, double now)
{

    move(app);

    // update model position
    object.WorldFromModel = translate(mat4(1), vec3(xPos, yPos, zPos));
    object.ModelFromWorld = inverse(object.WorldFromModel);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferIDs[OBJECT_UNIFORM_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ObjectData), &object);

    Object::draw(app, now);
}

