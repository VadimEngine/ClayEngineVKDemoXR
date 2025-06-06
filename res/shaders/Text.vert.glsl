#version 450

layout(location = 0) in vec4 inVertex; // xy = position, zw = tex coords
layout(location = 1) in int inGlyphIndex;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) flat out int glyphIndex;

layout(binding = 0) uniform ViewProj {
    mat4 uView;
    mat4 uProj;
};

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
} pc;

void main() {
    fragTexCoord = inVertex.zw;
    glyphIndex = inGlyphIndex;
    gl_Position = uProj * uView * pc.model * vec4(inVertex.xy, 0.0, 1.0);
}
