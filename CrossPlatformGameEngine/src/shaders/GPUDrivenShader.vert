#version 450

#extension GL_KHR_vulkan_glsl : enable

struct InstanceData {
    mat4 transform;
    uint textureId;
    uint pad[3];
};

layout(std140, set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(std430, set = 0, binding = 1) readonly buffer Instances {
    InstanceData instances[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint outTextureId;

void main() {
    const InstanceData instance = instances[gl_InstanceIndex];

    const mat4 cameraTransform = camera.proj * camera.view;

    gl_Position = cameraTransform * instance.transform * vec4(inPosition, 1.0);

    outTexCoord = vec2(inTexCoord.x, -inTexCoord.y);
    outTextureId = instance.textureId;
}
