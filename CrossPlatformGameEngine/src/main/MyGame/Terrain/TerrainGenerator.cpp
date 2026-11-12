#include "TerrainGenerator.h"

#include "BiomeGenerator.h"

void TerrainGenerator::generate(
    glm::ivec2 location,
    const uint32_t size,
    const uint32_t height,
    const uint32_t resolution,
    const uint32_t lod,
    const float frequency,
    const float uv,
    const TerrainNeighborLods& neighbors,
    TerrainData& data)
{
    auto& vertices = data.mVertices;
    auto& indices = data.mIndices;

    vertices.clear();
    indices.clear();

    const uint32_t lodResolution = std::max(1u, resolution >> std::min(lod, 31u));

    const uint32_t width = (lodResolution) + 1;
    const float step = static_cast<float>(size) / static_cast<float>(lodResolution);
    const float normalStep = static_cast<float>(size) / static_cast<float>(resolution);

    vertices.reserve(static_cast<size_t>(width) * width);

    indices.reserve(static_cast<size_t>(lodResolution) * lodResolution * 6);

    const float biomeFrequency = 0.001f;
    const uint32_t biomeSeed = 123454321;

    const auto sampleHeight = [&](float worldX, float worldZ)
    {
        const BiomeSample biome = BiomeGenerator::sample(
            worldX,
            worldZ,
            biomeFrequency,
            biomeSeed
        );

        float worldY = 0.0f;

        for (uint32_t i = 0; i < biome.mCellCount; ++i)
        {
            const BiomeCell& cell = biome.mCells[i];

            const BiomeType type = BiomeGenerator::getBiomeType(cell.mCellId, biomeSeed);
            const BiomeDefinition definition = BiomeGenerator::getBiomeDefinition(type);

            const float biomeNoise = noise(worldX * frequency * definition.mFrequencyScale, worldZ * frequency * definition.mFrequencyScale, 0);

            float biomeHeight;

            if (type == BiomeType::Mountains)
            {
                biomeHeight = biomeNoise * static_cast<float>(height) * definition.mHeightScale;
            }
            else
            {
                biomeHeight = (biomeNoise - 0.5f) * static_cast<float>(height) * definition.mHeightScale;
            }

            worldY += biomeHeight * cell.mWeight;
        }

        return worldY;
    };

    for (uint32_t z = 0; z <= lodResolution; z++)
    {
        for (uint32_t x = 0; x <= lodResolution; x++)
        {
            const float px = static_cast<float>(x) * step;
            const float pz = static_cast<float>(z) * step;

            const float worldX = static_cast<float>(location.x) * static_cast<float>(size) + px;
            const float worldZ = static_cast<float>(location.y) * static_cast<float>(size) + pz;

            const float worldY = sampleHeight(worldX, worldZ);

            // Sample neighboring heights for smooth normals
            const float hL = sampleHeight(worldX - normalStep, worldZ);
            const float hR = sampleHeight(worldX + normalStep, worldZ);
            const float hD = sampleHeight(worldX, worldZ - normalStep);
            const float hU = sampleHeight(worldX, worldZ + normalStep);

            const glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f * normalStep, hD - hU));

            TerrainVertex vertex{};
            vertex.mPosition = glm::vec3(worldX, worldY, worldZ);
            vertex.mNormal = normal;

            vertex.mTexCoords = glm::vec2(worldX * uv, worldZ * uv);

            vertices.push_back(vertex);
        }
    }

    for (uint32_t z = 0; z < lodResolution; ++z)
    {
        for (uint32_t x = 0; x < lodResolution; ++x)
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

    const auto neighborResolution = [&](uint32_t neighborLod)
    {
        return std::max(
            1u,
            resolution >> std::min(neighborLod, 31u)
        );
    };

    stitchEdge(
        vertices,
        lodResolution,
        neighborResolution(neighbors.mLeft),
        width,
        false,
        false
    );

    stitchEdge(
        vertices,
        lodResolution,
        neighborResolution(neighbors.mRight),
        width,
        false,
        true
    );

    stitchEdge(
        vertices,
        lodResolution,
        neighborResolution(neighbors.mDown),
        width,
        true,
        false
    );

    stitchEdge(
        vertices,
        lodResolution,
        neighborResolution(neighbors.mUp),
        width,
        true,
        true
    );
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

void TerrainGenerator::stitchEdge(
    std::vector<TerrainVertex>& vertices,
    const uint32_t lodResolution,
    const uint32_t neighborResolution,
    const uint32_t width,
    const bool alongX,
    const bool positiveSide)
{
    if (neighborResolution >= lodResolution) return;
    if (lodResolution % neighborResolution != 0) return;

    const uint32_t ratio = lodResolution / neighborResolution;

    auto vertexIndex = [&](uint32_t i) -> uint32_t
    {
        if (alongX)
        {
            const uint32_t z = positiveSide ? lodResolution : 0;
            return z * width + i;
        }
        else
        {
            const uint32_t x = positiveSide ? lodResolution : 0;
            return i * width + x;
        }
    };

    for (uint32_t i = 1; i < lodResolution; ++i)
    {
        if (i % ratio == 0) continue;

        const uint32_t lower = (i / ratio) * ratio;
        const uint32_t upper = lower + ratio;

        const float t = static_cast<float>(i - lower) / static_cast<float>(ratio);

        const float h0 = vertices[vertexIndex(lower)].mPosition.y;
        const float h1 = vertices[vertexIndex(upper)].mPosition.y;

        vertices[vertexIndex(i)].mPosition.y = glm::mix(h0, h1, t);
    }
}

