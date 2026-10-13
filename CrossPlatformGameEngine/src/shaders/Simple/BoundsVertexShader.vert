#version 450

#extension GL_KHR_vulkan_glsl : enable

layout(push_constant) uniform PushConstants
{
    mat4 cameraVP;
} pushConstants;

layout(location = 0) in vec4 inPosition;

void main()
{
    gl_Position = pushConstants.cameraVP * inPosition;
}
