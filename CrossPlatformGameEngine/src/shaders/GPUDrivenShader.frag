#version 450

//layout(binding = 2) uniform sampler2DArray texArray;
layout(binding = 2) uniform sampler2D tex;

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // int texIndex = renderElements[0].textureId;
    // outColor = texture(texArray, vec3(texCoord, texIndex));
    outColor = texture(tex, inTexCoord);
}