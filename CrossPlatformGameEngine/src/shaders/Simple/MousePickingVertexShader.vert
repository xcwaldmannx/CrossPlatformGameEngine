#version 450

struct Entity
{
    vec3 position;
    uint entityId;

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

layout(std430, set = 0, binding = 0) readonly buffer Entities
{
    Entity entities[];
};

layout(push_constant) uniform PushConstants
{
    mat4 cameraVP;
} pushConstants;

layout(location = 0) in vec3 inPosition;

layout(location = 0) flat out uint outEntityId;

mat4 eulerRotationToMat4(vec3 euler)
{
    float cx = cos(euler.x);
    float sx = sin(euler.x);
    float cy = cos(euler.y);
    float sy = sin(euler.y);
    float cz = cos(euler.z);
    float sz = sin(euler.z);

    mat3 rz = mat3(
         cz, -sz, 0.0,
         sz,  cz, 0.0,
        0.0, 0.0, 1.0);

    mat3 ry = mat3(
         cy, 0.0,  sy,
        0.0, 1.0, 0.0,
        -sy, 0.0, cy);

    mat3 rx = mat3(
        1.0, 0.0, 0.0,
        0.0,  cx, -sx,
        0.0,  sx,  cx);

    mat4 result = mat4(1.0);
    result[0].xyz = (rz * ry * rx)[0];
    result[1].xyz = (rz * ry * rx)[1];
    result[2].xyz = (rz * ry * rx)[2];

    return result;
}

mat4 buildTransform(vec3 position, vec3 rotation, vec3 scale)
{
    mat4 translation = mat4(1.0);
    translation[3].xyz = position;

    mat4 scaling = mat4(1.0);
    scaling[0][0] = scale.x;
    scaling[1][1] = scale.y;
    scaling[2][2] = scale.z;

    return translation *
           eulerRotationToMat4(rotation) *
           scaling;
}

void main()
{
    Entity entity = entities[gl_InstanceIndex];

    if (entity.isVisible == 0)
    {
        gl_Position = vec4(0.0);
        outEntityId = 0;
        return;
    }

    mat4 model = buildTransform(
        entity.position,
        entity.rotation,
        entity.scale);

    gl_Position =
        pushConstants.cameraVP *
        model *
        vec4(inPosition, 1.0);

    // Zero is reserved for background.
    outEntityId = entity.entityId + 1;
}