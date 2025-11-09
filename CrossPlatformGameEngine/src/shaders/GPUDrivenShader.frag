#version 450

layout(binding = 2) uniform sampler2DArray texArray;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in flat uint inTextureId;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texArray, vec3(inTexCoord, inTextureId));
}