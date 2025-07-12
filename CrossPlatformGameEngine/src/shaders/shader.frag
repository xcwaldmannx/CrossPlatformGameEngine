#version 450

struct PerEntityData {
    mat4 transform;
    int textureId;
};

layout(std430, binding = 1) readonly buffer perEntityDataArray {
    PerEntityData[] entities;
};

layout(binding = 2) uniform sampler2DArray texArray;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoord;
layout(location = 2) flat in int instanceId;

layout(location = 0) out vec4 outColor;

void main() {
    int texIndex = entities[instanceId].textureId;
    outColor = texture(texArray, vec3(texCoord, texIndex));
}