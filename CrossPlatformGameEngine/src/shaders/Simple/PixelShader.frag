#version 450
#extension GL_EXT_samplerless_texture_functions : require

layout(set = 0, binding = 0x02) uniform sampler texSampler;
layout(set = 0, binding = 0x03) uniform texture2DArray texArray;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in flat uint inTextureId;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(sampler2DArray(texArray, texSampler), vec3(inTexCoord, float(inTextureId)));
}
