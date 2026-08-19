#version 450

struct Entity
{
    vec3 position;
    uint id;
    vec4 rotation;
    vec3 scale;
    uint isSelected;

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

mat4 quaternionToMat4(vec4 q)
{
    q = normalize(q);

    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;

    float xy = x * y;
    float xz = x * z;
    float yz = y * z;

    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    mat4 R = mat4(1.0);

    R[0][0] = 1.0 - 2.0 * (yy + zz);
    R[0][1] = 2.0 * (xy + wz);
    R[0][2] = 2.0 * (xz - wy);

    R[1][0] = 2.0 * (xy - wz);
    R[1][1] = 1.0 - 2.0 * (xx + zz);
    R[1][2] = 2.0 * (yz + wx);

    R[2][0] = 2.0 * (xz + wy);
    R[2][1] = 2.0 * (yz - wx);
    R[2][2] = 1.0 - 2.0 * (xx + yy);

    return R;
}

mat4 buildTransform(vec3 pos, vec4 rot, vec3 scale)
{
    mat4 T = mat4(1.0);
    T[3].xyz = pos;

    mat4 R = quaternionToMat4(rot);

    mat4 S = mat4(1.0);
    S[0][0] = scale.x;
    S[1][1] = scale.y;
    S[2][2] = scale.z;

    return T * R * S;
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
    outEntityId = entity.id + 1;
}