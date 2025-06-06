#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D glyphTextures[128];

layout(location = 1) flat in int glyphIndex;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
} pc;

void main() {
    float intensity = texture(glyphTextures[glyphIndex], fragTexCoord).r;
    outColor = vec4(intensity) * pc.color; // replicate r to RGB
}
