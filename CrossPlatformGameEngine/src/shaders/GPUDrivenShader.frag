#version 450
#extension GL_EXT_samplerless_texture_functions : require

layout(set = 1, binding = 0x10) uniform sampler texSampler;
layout(set = 1, binding = 0x11) uniform TextureDArray texArray;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in flat uint inTextureId;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(SamplerDArray(texArray, texSampler), vec3(inTexCoord, float(inTextureId)));
}
