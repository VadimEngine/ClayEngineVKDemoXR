#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
} pc;

void main() {
    vec2 flippedTexCoords = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);
    outColor = texture(texSampler, flippedTexCoords) * pc.color;
}