#version 450

#extension GL_KHR_vulkan_glsl : enable

struct RenderElementStruct {
    int transformOffset;
    int textureId;
    int _pad0;
    int _pad1;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(std430, binding = 1) readonly buffer RenderElementsBuffer {
    RenderElementStruct renderElements[];
};

layout(std430, binding = 2) readonly buffer TransformsBuffer {
    mat4 transforms[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoord;

void main() {
    const RenderElementStruct elem = renderElements[0];

    gl_Position = ubo.proj * ubo.view * transforms[0] * vec4(inPosition, 1.0);
    fragColor = inColor;
    texCoord = vec2(inTexCoord.x, -inTexCoord.y);
}
