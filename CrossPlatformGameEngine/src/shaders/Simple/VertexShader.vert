#version 450

#extension GL_KHR_vulkan_glsl : enable

struct Entity
{
    vec3 position;
    uint _pad0;

    vec3 rotation;
    uint _pad1;

    vec3 scale;

    uint isVisible;

    uint meshCount;
    uint meshOffset;
    uint _pad2[2];
};

layout(std140, set = 0, binding = 0x00) uniform Camera
{
    mat4 view;
    mat4 proj;
} camera;

layout(std430, set = 0, binding = 0x01) readonly buffer Entities
{
    Entity entities[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint outTextureId;

void main()
{

}
