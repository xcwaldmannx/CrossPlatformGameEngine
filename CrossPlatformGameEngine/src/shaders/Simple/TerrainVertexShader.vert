#version 450

layout(push_constant) uniform PushConstants
{
    mat4 cameraVP;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTexCoord;

void main()
{
    gl_Position = pushConstants.cameraVP * vec4(inPosition, 1.0);

    outNormal = inNormal;
    outTexCoord = vec2(inTexCoord.x, -inTexCoord.y);
}
