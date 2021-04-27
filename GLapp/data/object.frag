#version 410 core
// simple object fragment shader

// per-frame data
layout(std140)                  // standard layout
uniform SceneData {             // like a class name
    mat4 ProjFromWorld, WorldFromProj;
    vec4 LightDir;
};

// shader settings
uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;
uniform sampler2D PropsTexture;

// input from vertex shader
in vec2 texcoord;
in vec3 normal;
in vec4 position;

// output to frame buffer
out vec4 fragColor;

void main() {

    // solid color or from texture
    vec3 color = vec3(.5);
    if(textureSize(ColorTexture, 0) != ivec2(1, 1))
        color = texture(ColorTexture, 4. * texcoord).rgb;

    float ao = texture(PropsTexture, texcoord).b;
    float ambient = LightDir.a * ao;

    vec3 N = normalize(normal);             // surface normal
    vec3 L = normalize(LightDir.xyz);       // light direction
    float diffuse = max(0., dot(N,L));      // diffuse lighting
    float I = min(1., diffuse + ambient);   // add in ambient

    vec4 eyeProj = vec4(0,0,-1,0);
    vec4 eyeWorld = WorldFromProj * eyeProj;
    vec3 V = eyeWorld.xyz/eyeWorld.w - position.xyz/position.w;
    V = normalize(V);
    vec3 H = normalize(V + L);

    float gloss = texture(PropsTexture, texcoord).r;
    float fresnel = .04 + (1. - .04) * pow(1. - dot(N, V), 5.);
    float specPower = pow(2, 12 * gloss);
    float specular = pow(dot(N,H), specPower) * (specPower + 1) / 2;
    
    //I += specular;

    // color from texture
    color = I * texture(ColorTexture, texcoord).rgb;

    // final color
    fragColor = vec4(color, 1);
}
