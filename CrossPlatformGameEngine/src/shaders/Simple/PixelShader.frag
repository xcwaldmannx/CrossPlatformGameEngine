#version 450
#extension GL_EXT_samplerless_texture_functions : require
#extension GL_EXT_nonuniform_qualifier : require

struct Material
{
    vec4 baseColor;

    float metallic;
    float roughness;

    int baseColorTextureId;
    int normalTextureId;

    int metallicRoughnessTextureId;
    int occlusionTextureId;
    int emissiveTextureId;
};

layout(set = 0, binding = 0x03) uniform sampler texSampler;
layout(set = 0, binding = 0x04) uniform texture2D textures[1024];

layout(std430, set = 0, binding = 0x05) readonly buffer Materials
{
    Material materials[];
};

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in flat uint inMaterialId;
layout(location = 2) in flat uint inSelected;

layout(location = 0) out vec4 outColor;

void main()
{
    Material material = materials[inMaterialId];

    vec4 t;

    if (material.baseColorTextureId >= 0)
    {
        t = texture(sampler2D(textures[nonuniformEXT(material.baseColorTextureId)], texSampler), inTexCoord);
    }
    else
    {
        t = texture(sampler2D(textures[nonuniformEXT(1)], texSampler), inTexCoord);
    }

    if (inSelected > 0)
    {
        outColor = mix(vec4(1, 0, 0, 1), t, 0.75);
    }
    else
    {
        outColor = t;
    }
}
