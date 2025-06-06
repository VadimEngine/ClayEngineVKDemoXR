#version 450

layout(location = 0) in vec3 FaceNormal;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
} pc;

void main() {
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

    vec3 normal = normalize(FaceNormal);

    // Simple Lambertian shading (diffuse only)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 color = pc.color.rgb * diff + vec3(.95, .674, .411) * 0.2; // Adding some ambient light
    //vec3(.95, .674, .411)
    outColor = vec4(color, 1.0);
}