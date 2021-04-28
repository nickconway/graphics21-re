#version 410 core
// simple object vertex shader

// per-frame data
layout(std140)                  // standard layout
uniform SceneData {             // like a class name
    mat4 ProjFromWorld, WorldFromProj;
    vec4 LightDir;
    int colorEnabled;
    int normalEnabled;
    int glossEnabled;
    int aoEnabled;
};

// per-object data
layout(std140)
uniform ObjectData {
    mat4 WorldFromModel, ModelFromWorld;
};

// per-vertex input
in vec2 vUV;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vT;
in vec3 vB;

// output to fragment shader (view space)
out vec2 texcoord;
out vec3 normal;
out vec4 position;
out mat3 TBN;

void main() {
    texcoord = vUV;
    position = WorldFromModel * vec4(vPosition, 1);

    normal = normalize(vNormal * mat3(ModelFromWorld));
    vec3 tangent = normalize(vT * mat3(WorldFromModel));
    vec3 bitangent = normalize(cross(normal, tangent));
    TBN = mat3(tangent, bitangent, normal);

    gl_Position = ProjFromWorld * position;
}
