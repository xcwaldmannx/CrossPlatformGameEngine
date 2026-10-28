#version 450
#extension GL_EXT_samplerless_texture_functions : require
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0x01) uniform sampler texSampler;
layout(set = 0, binding = 0x02) uniform texture2D textures[1024];

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 t = vec4(1, 1, 1, 1);

    t *= texture(sampler2D(textures[nonuniformEXT(0)], texSampler), inTexCoord);

    outColor = t;
}
