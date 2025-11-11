#version 450

#extension GL_KHR_vulkan_glsl : enable

struct InstanceData {
    mat4 modelTransform;
    mat4 meshTransform;
    uint transformId;
    uint textureId;
    uint pad[2];
};

layout(std140, set = 0, binding = 0x00) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(std430, set = 0, binding = 0x01) readonly buffer Transforms {
    mat4 transforms[];
};

layout(std430, set = 0, binding = 0x02) readonly buffer Instances {
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
    
    const mat4 T_model = instance.modelTransform; // the model
    const mat4 T_mesh  = instance.meshTransform;  // the dynamic
    const mat4 T_local = transforms[instance.transformId]; // the mesh local

    const mat4 finalTransform = T_model * T_local * T_mesh;

    gl_Position = cameraTransform * finalTransform * vec4(inPosition, 1.0);

    outTexCoord = vec2(inTexCoord.x, -inTexCoord.y);
    outTextureId = instance.textureId;
}
