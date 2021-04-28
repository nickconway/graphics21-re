// base class for drawable objects
#pragma once

#include "Shader.hpp"
#include <glm/glm.hpp>
#include <vector>

class Object {
public:
    // uniform buffer data about the object position
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct ObjectData {
        glm::mat4 WorldFromModel, ModelFromWorld;
    } object;

    // arrays defining triangles for GPU
    unsigned int varrayID;              // GL vertex array object, containing:
    std::vector<glm::vec3> vert;        //   per-vertex position
    std::vector<glm::vec3> norm;        //   per-vertex normal
    std::vector<glm::vec2> uv;          //   per-vertex texture coordinate
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<unsigned int> indices;  //   3 vertex indices per triangle

    // GL texture ID(s), array for extensibility to more textures
    enum {COLOR_TEXTURE, NORMAL_TEXTURE, PROPS_TEXTURE, NUM_TEXTURES};
    unsigned int textureIDs[NUM_TEXTURES];

    // GL buffer object IDs
    enum {OBJECT_UNIFORM_BUFFER, POSITION_BUFFER, NORMAL_BUFFER, UV_BUFFER, TANGENT_BUFFER, BITANGENT_BUFFER, INDEX_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];

    // GL shaders
    unsigned int shaderID;      // ID for shader program
    std::vector<ShaderInfo> shaderParts;  // vertex & fragment shader info

    // base object constructor: create buffers and textures
    Object(std::vector<const char*> textures);

    // virtual destructor to delete any child class data
    virtual ~Object();

    // load/reload shaders
    virtual void updateShaders();

    // draw this object
    virtual void draw(class GLapp *app, double now);

    // load an image file into a texture object
    void loadPPM(const char *imagefile, unsigned int bufferID);

    void computeTangents();

};