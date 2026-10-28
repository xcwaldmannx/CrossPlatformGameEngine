#include "TerrainGenerator.h"

void TerrainGenerator::generate(
    glm::ivec2 location,
    const uint32_t size,
    const uint32_t height,
    const uint32_t resolution,
    const float frequency,
    const float uv,
    TerrainData& data)
{
    auto& vertices = data.mVertices;
    auto& indices = data.mIndices;

    vertices.clear();
    indices.clear();

    const uint32_t width = resolution + 1;
    const float step = static_cast<float>(size) / static_cast<float>(resolution);

    vertices.reserve(static_cast<size_t>(width) * width);

    indices.reserve(static_cast<size_t>(resolution) * resolution * 6);

    const auto sampleHeight = [&](float worldX, float worldZ)
    {
        return (noise(worldX * frequency, worldZ * frequency, 0) - 0.5f) * static_cast<float>(height);
    };

    for (uint32_t z = 0; z <= resolution; z++)
    {
        for (uint32_t x = 0; x <= resolution; x++)
        {
            const float px = static_cast<float>(x) * step;
            const float pz = static_cast<float>(z) * step;

            const float worldX = static_cast<float>(location.x) * static_cast<float>(size) + px;
            const float worldZ = static_cast<float>(location.y) * static_cast<float>(size) + pz;

            const float worldY = sampleHeight(worldX, worldZ);

            // Sample neighboring heights for smooth normals
            const float hL = sampleHeight(worldX - step, worldZ);
            const float hR = sampleHeight(worldX + step, worldZ);
            const float hD = sampleHeight(worldX, worldZ - step);
            const float hU = sampleHeight(worldX, worldZ + step);

            const glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f * step, hD - hU));

            TerrainVertex vertex{};
            vertex.mPosition = glm::vec3(worldX, worldY, worldZ);
            vertex.mNormal = normal;

            vertex.mTexCoords = glm::vec2(worldX * uv, worldZ * uv);

            vertices.push_back(vertex);
        }
    }

    for (uint32_t z = 0; z < resolution; ++z)
    {
        for (uint32_t x = 0; x < resolution; ++x)
        {
            const uint32_t i = z * width + x;

            indices.push_back(i);
            indices.push_back(i + width);
            indices.push_back(i + 1);

            indices.push_back(i + 1);
            indices.push_back(i + width);
            indices.push_back(i + width + 1);
        }
    }
}

float TerrainGenerator::noise(const float x, const float y, const uint32_t seed)
{
    const int32_t ix = std::floor(x);
    const int32_t iy = std::floor(y);

    const float fx = x - static_cast<float>(ix);
    const float fy = y - static_cast<float>(iy);

    const auto hashFloat = [](const int32_t hx, const int32_t hy, const uint32_t seed)
    {
        return static_cast<float>(hash(hx, hy, seed) >> 8) * (1.0f / 16777216.0f);
    };

    const float h00 = hashFloat(ix, iy, seed);
    const float h10 = hashFloat(ix + 1, iy, seed);
    const float h01 = hashFloat(ix, iy + 1, seed);
    const float h11 = hashFloat(ix + 1, iy + 1, seed);

    const auto fade = [](const float t)
    {
        return t * t * (3.0f - 2.0f * t);
    };

    const float u = fade(fx);
    const float v = fade(fy);

    return glm::mix(glm::mix(h00, h10, u), glm::mix(h01, h11, u), v);
}

uint32_t TerrainGenerator::hash(const int32_t x, const int32_t y, const uint32_t seed)
{
    uint32_t h = 2166136261u;

    h = (h ^ seed) * 16777619u;
    h = (h ^ static_cast<uint32_t>(x)) * 16777619u;
    h = (h ^ static_cast<uint32_t>(y)) * 16777619u;

    h ^= h >> 16;
    h *= 0x7feb352du;
    h ^= h >> 15;
    h *= 0x846ca68bu;
    h ^= h >> 16;

    return h;
}