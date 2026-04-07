#version 450

#extension GL_KHR_vulkan_glsl : enable

struct Entity
{
    vec3 position;
    uint _pad0;
    vec3 rotation;
    uint _pad1;
    vec3 scale;
    uint _pad2;

    vec3 boundsPos;
    uint _pad3;
    vec3 boundsNeg;

    uint isVisible;

    uint meshCount;
    uint meshOffset;
    uint _pad4[2];
};

layout(std430, set = 0, binding = 0x00) readonly buffer Entities
{
    Entity entities[];
};

layout(push_constant) uniform PushConstants
{
    mat4 cameraVP;
} pushConstants;

mat4 eulerRotationToMat4(vec3 euler)
{
    float cx = cos(euler.x);
    float sx = sin(euler.x);
    float cy = cos(euler.y);
    float sy = sin(euler.y);
    float cz = cos(euler.z);
    float sz = sin(euler.z);

    mat3 Rz = mat3(
        cz, -sz, 0.0,
        sz,  cz, 0.0,
        0.0, 0.0, 1.0
    );

    mat3 Ry = mat3(
         cy, 0.0, sy,
         0.0, 1.0, 0.0,
        -sy, 0.0, cy
    );

    mat3 Rx = mat3(
        1.0, 0.0, 0.0,
        0.0,  cx, -sx,
        0.0,  sx,  cx
    );

    mat3 R = Rz * Ry * Rx;

    mat4 M = mat4(1.0);
    M[0].xyz = R[0];
    M[1].xyz = R[1];
    M[2].xyz = R[2];
    return M;
}

mat4 buildTransform(vec3 pos, vec3 rot, vec3 scale)
{
    mat4 T = mat4(1.0);
    T[3].xyz = pos;

    mat4 R = eulerRotationToMat4(rot);

    mat4 S = mat4(1.0);
    S[0][0] = scale.x;
    S[1][1] = scale.y;
    S[2][2] = scale.z;

    return T * R * S;
}

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint outTextureId;

void main()
{
    Entity entity = entities[gl_InstanceIndex];

    if (entity.isVisible == 0)
    {
        gl_Position = vec4(0.0);
    }
    else
    {
        const mat4 entityTransform = buildTransform(entity.position, entity.rotation, entity.scale);

        gl_Position = pushConstants.cameraVP * entityTransform * vec4(inPosition, 1.0);
    }

    outTexCoord = vec2(inTexCoord.x, -inTexCoord.y);
    outTextureId = 0;
}
