// base class for drawable objects

#include "Object.hpp"
#include "GLapp.hpp"
#include "config.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>

using namespace glm;  // avoid glm:: for all glm types and functions

#ifdef _WIN32
// don't complain if we use standard IO functions instead of windows-only
#pragma warning( disable: 4996 )
#endif

Object::Object(std::vector<const char*> textures)
{

    // create buffer objects to be used later
    glGenTextures(NUM_TEXTURES, textureIDs);
    glGenBuffers(NUM_BUFFERS, bufferIDs);
    glGenVertexArrays(1, &varrayID);

    // load color image into a named texture
    for (int i = 0; i < textures.size(); i++) {
        loadPPM(textures[i], textureIDs[i]);
    }

    // default to position at origin
    object = {mat4(1), mat4(1)};
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[OBJECT_UNIFORM_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ObjectData), &object, GL_STREAM_DRAW);

    // initial shader load
    shaderParts = {
        {glCreateShader(GL_VERTEX_SHADER  ), "object.vert"},
        {glCreateShader(GL_FRAGMENT_SHADER), "object.frag"}
    };
    shaderID = glCreateProgram();

}

Object::~Object()
{
    for (auto shader : shaderParts)
       glDeleteShader(shader.id);
    glDeleteProgram(shaderID);
    glDeleteTextures(NUM_TEXTURES, textureIDs);
    glDeleteBuffers(NUM_BUFFERS, bufferIDs);
    glDeleteVertexArrays(1, &varrayID);
}

// load or replace object shaders
void Object::updateShaders()
{
    loadShaders(shaderID, shaderParts);
    glUseProgram(shaderID);

    // Bind uniform block #s to their shader names. Indices should match glBindBufferBase in draw
    glUniformBlockBinding(shaderID, glGetUniformBlockIndex(shaderID,"SceneData"),  0);
    glUniformBlockBinding(shaderID, glGetUniformBlockIndex(shaderID,"ObjectData"), 1);

    // Map shader name for texture. Index# should match GL_TEXTURE# used in draw
    glUniform1i(glGetUniformLocation(shaderID, "ColorTexture"), 0);
    glUniform1i(glGetUniformLocation(shaderID, "NormalTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderID, "PropsTexture"), 2);

    // bind attribute arrays
    glBindVertexArray(varrayID);

    GLint positionAttrib = glGetAttribLocation(shaderID, "vPosition");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);

    GLint normalAttrib = glGetAttribLocation(shaderID, "vNormal");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalAttrib);

    GLint uvAttrib = glGetAttribLocation(shaderID, "vUV");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(uvAttrib);

    GLint tAttrib = glGetAttribLocation(shaderID, "vT");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[TANGENT_BUFFER]);
    glVertexAttribPointer(tAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(tAttrib);

    GLint bAttrib = glGetAttribLocation(shaderID, "vB");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[BITANGENT_BUFFER]);
    glVertexAttribPointer(bAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(bAttrib);
}

void Object::loadPPM(const char *imagefile, unsigned int bufferID)
{
    // open file in project data directory
    std::string path = std::string(PROJECT_DATA_DIR) + imagefile;
    FILE *fp = fopen(path.c_str(), "rb");
    assert(fp);

    // check that "magic number" at beginning of file is P6
    if (fgetc(fp) != 'P' || fgetc(fp) != '6') {
        fprintf(stderr, "unknown image format %s\n", path.c_str());
        assert(false);
    }

    // read image size, maximum value, and blank following header
    int width = 0, height = 0, maxval = 0, lf = 0;
    fscanf(fp, " #%*[^\n]");                // skip comment (if there)
    fscanf(fp, "%d %d", &width, &height);   // read image size
    assert(width > 0 && height > 0);

    fscanf(fp, " #%*[^\n]");                // skip comment (if there)
    fscanf(fp, "%d", &maxval);              // skip max value
    assert(maxval == 255);

    lf = fgetc(fp);                         // skip final \n before data
    assert(lf == '\n');

    // check remaining file size matches image size
    // if this fails, you may have checked a ppm file out
    // as text rather than binary
    long headerEnd = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long fileEnd = ftell(fp);
    fseek(fp, headerEnd, SEEK_SET);
    assert(fileEnd - headerEnd == width*height*3);

    // allocate image and read array
    u8vec3 *image = new u8vec3[width * height];
    fread(image, sizeof(u8vec3), width * height, fp);
    fclose(fp);

    // load into texture
    glBindTexture(GL_TEXTURE_2D, bufferID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void Object::draw(GLapp* app, double now)
{
    // enable shader
    glUseProgram(shaderID);

    // select vertex array to render
    glBindVertexArray(varrayID);

    // bind color texture to active texture #0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIDs[COLOR_TEXTURE]);
    glDisable(GL_TEXTURE_2D);

    // bind normal texture to active texture #1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureIDs[NORMAL_TEXTURE]);

    // bind props texture to active texture #2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureIDs[PROPS_TEXTURE]);

    // bind uniform buffers to the appropriate uniform block numbers
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, app->sceneUniformsID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferIDs[OBJECT_UNIFORM_BUFFER]);

    // draw the triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Object::computeTangents() {

    for (int i = 0; i < indices.size(); i += 3) {

        vec3 v0 = vert[indices[i]];
        vec3 v1 = vert[indices[i+1]];
        vec3 v2 = vert[indices[i+2]];

        vec2 uv0 = uv[indices[i]];
        vec2 uv1 = uv[indices[i + 1]];
        vec2 uv2 = uv[indices[i + 2]];

        vec3 deltaPos1 = v1 - v0;
        vec3 deltaPos2 = v2 - v0;

        vec2 deltaUV1 = uv1 - uv0;
        vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        tangents.push_back(normalize(tangent - norm[indices[i]] * dot(norm[indices[i]], tangent)));
        tangents.push_back(normalize(tangent - norm[indices[i + 1]] * dot(norm[indices[i + 1]], tangent)));
        tangents.push_back(normalize(tangent - norm[indices[i + 2]] * dot(norm[indices[i + 2]], tangent)));

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);

    }

    std::vector<vec3> newTangents;
    std::vector<vec3> newBitangents;
    // Go through each vertex
    for (auto vertex : vert) {

        vec3 tangentSum(0, 0, 0);
        vec3 bitangentSum(0, 0, 0);
        int count = 0;

        // Go through each triangle
        for (int i = 0; i < indices.size(); i += 3) {
            std::vector<vec3> triangle = { vert[indices[i]], vert[indices[i + 1]], vert[indices[i + 2]] };
            for (auto v : triangle) {

                // If vertex is in the triangle
                if (vertex == v) {

                    tangentSum += tangents[i];
                    count++;

                }

            }

        }

        newTangents.push_back(normalize(tangentSum));
        newBitangents.push_back(normalize(bitangentSum));

    }

    //tangents = newTangents;
    //bitangents = newBitangents;

}
