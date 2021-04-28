#version 410 core
// simple object fragment shader

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

    float ambient = LightDir.a;
    if(aoEnabled == 1)
        ambient *= texture(PropsTexture, texcoord).b;

    vec3 N = normalize(normal);             // surface normal
    vec3 L = normalize(LightDir.xyz);       // light direction
    float diffuse = max(0., dot(N,L));      // diffuse lighting
    float I = min(1, diffuse + ambient);   // add in ambient

    vec4 eyeProj = vec4(0,0,-1,0);
    vec4 eyeWorld = WorldFromProj * eyeProj;
    vec3 V = eyeWorld.xyz/eyeWorld.w - position.xyz/position.w;
    V = normalize(V);
    vec3 H = normalize(V + L);

    float fresnel = .04 + .96 * pow(1 - dot(N, V), 5);
    float specPower = 64;
    if(glossEnabled == 1)
        specPower = pow(2, 12 * texture(PropsTexture, texcoord).r);
    float specular = pow(dot(N,H), specPower) * (specPower + 1) / 2;

    I  += mix(I, specular, fresnel);

    // color from texture
    vec3 color = I * vec3(.5);
    if(colorEnabled == 1)
        color = I * texture(ColorTexture, texcoord).rgb;

    // final color
    fragColor = vec4(color, 1);
}
