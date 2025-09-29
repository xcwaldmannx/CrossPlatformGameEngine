#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

struct PerEntityData {
    mat4 transform;
    int modelId;
    int textureId;
    int _pad0;
    int _pad1;
};

layout(std430, binding = 1) readonly buffer perEntityDataArray {
    PerEntityData[] entities;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoord;
layout(location = 2) flat out int instanceId;

void main() {
    gl_Position = ubo.proj * ubo.view * entities[gl_InstanceIndex].transform * vec4(inPosition, 1.0);
    fragColor = inColor;
    texCoord = vec2(inTexCoord.x, inTexCoord.y * -1);
    instanceId = gl_InstanceIndex;
}