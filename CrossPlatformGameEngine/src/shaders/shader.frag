#version 450

struct RenderElementStruct {
    int transformOffset;
    int textureId;
    int _pad0;
    int _pad1;
};

layout(std430, binding = 1) readonly buffer renderElementsBuffer {
    RenderElementStruct renderElements[];
};

//layout(binding = 3) uniform SamplerDArray texArray;
layout(binding = 3) uniform SamplerD tex;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

void main() {
    int texIndex = renderElements[0].textureId;
    //outColor = texture(texArray, vec3(texCoord, texIndex));
    outColor = texture(tex, texCoord);
}