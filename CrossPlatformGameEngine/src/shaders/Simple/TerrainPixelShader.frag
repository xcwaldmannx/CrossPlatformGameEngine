#version 450
#extension GL_EXT_samplerless_texture_functions : require
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0x01) uniform sampler texSampler;
layout(set = 0, binding = 0x02) uniform texture2D textures[1024];

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

struct Sun
{
    vec3 direction;
    vec3 color;
    float intensity;
};

vec3 calculateSunlight(Sun sun, vec3 normal, vec3 albedo)
{
    vec3 N = normalize(normal);
    vec3 L = normalize(sun.direction);

    float NdotL = max(dot(N, L), 0.0);

    vec3 diffuse = albedo * sun.color * sun.intensity * NdotL;

    return diffuse;
}

void main()
{
    vec4 t = vec4(1, 1, 1, 1);

    Sun sun = Sun(normalize(vec3(0, 1, 1)), vec3(1, 0.8f, 0.8f), 1.0f);

    vec3 albedo = texture(sampler2D(textures[nonuniformEXT(0)], texSampler), inTexCoord).rgb;

    vec3 sunlight = calculateSunlight(sun, inNormal, albedo);

    t *= vec4(sunlight, 1);

    outColor = t;
}
