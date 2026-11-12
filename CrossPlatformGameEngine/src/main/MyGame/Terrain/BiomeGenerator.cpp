#include "BiomeGenerator.h"

#include <algorithm>

BiomeSample BiomeGenerator::sample(const float worldX, const float worldZ, const float frequency, const uint32_t seed)
{
    return noise(worldX * frequency, worldZ * frequency, seed);
}


BiomeType BiomeGenerator::getBiomeType(const glm::ivec2 cellId, const uint32_t seed)
{
    const uint32_t value = hash(cellId.x, cellId.y, seed ^ 0x9e3779b9u);

    return static_cast<BiomeType>(value % static_cast<uint32_t>(BiomeType::Count));
}

BiomeDefinition BiomeGenerator::getBiomeDefinition(const BiomeType type)
{
    switch (type) {
        case BiomeType::Plains:
            return { 0.15f, 0.5f };

        case BiomeType::Forest:
            return { 0.35f, 1.0f };

        case BiomeType::Desert:
            return { 0.20f, 0.7f };

        case BiomeType::Mountains:
            return { 2.0f, 0.5f };

        case BiomeType::Tundra:
            return { 0.40f, 1.2f };

        default:
            return { 1.0f, 1.0f };
    }
}

BiomeSample BiomeGenerator::noise(const float x, const float y, const uint32_t seed)
{
    const int32_t ix = static_cast<int32_t>(std::floor(x));
    const int32_t iy = static_cast<int32_t>(std::floor(y));

    const auto hashFloat = [](const int32_t hx, const int32_t hy, const uint32_t seed)
    {
        return static_cast<float>(hash(hx, hy, seed) >> 8) * (1.0f / 16777216.0f);
    };

    BiomeSample result{};

    constexpr int32_t searchRadius = 2;
    constexpr float blendRadius = 1.5f;

    float totalWeight = 0.0f;

    for (int32_t dy = -searchRadius; dy <= searchRadius; ++dy)
    {
        for (int32_t dx = -searchRadius; dx <= searchRadius; ++dx)
        {
            const int32_t cx = ix + dx;
            const int32_t cy = iy + dy;

            const float px = static_cast<float>(cx) + hashFloat(cx, cy, seed);
            const float py = static_cast<float>(cy) + hashFloat(cx, cy, seed + 1);

            const float deltaX = x - px;
            const float deltaY = y - py;

            const float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);

            // Compact-support influence.
            // Reaches zero at blendRadius.
            const float t = std::clamp(1.0f - distance / blendRadius, 0.0f, 1.0f);

            // Smoothstep with zero slope at both ends.
            const float weight = t * t * (3.0f - 2.0f * t);

            // Cells outside the influence radius
            // contribute nothing.
            if (weight <= 0.0f) continue;

            BiomeCell& cell = result.mCells[result.mCellCount++];

            cell.mCellId = { cx, cy };
            cell.mCellPosition = { px, py };
            cell.mDistance = distance * distance;
            cell.mWeight = weight;

            totalWeight += weight;
        }
    }

    // Normalize all contributing cells.
    if (totalWeight > 0.0f)
    {
        const float invTotalWeight = 1.0f / totalWeight;

        for (uint32_t i = 0; i < result.mCellCount; ++i)
        {
            result.mCells[i].mWeight *= invTotalWeight;
        }
    }

    return result;
}

uint32_t BiomeGenerator::hash(const int32_t x, const int32_t y, const uint32_t seed)
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
