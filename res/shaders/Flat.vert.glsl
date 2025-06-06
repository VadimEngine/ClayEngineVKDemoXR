#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
} pc;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec3 FaceNormal;


void main() {
    gl_Position = ubo.proj * ubo.view * pc.model * vec4(aPosition, 1.0);
    FaceNormal = mat3(transpose(inverse(pc.model))) * aNormal;
}