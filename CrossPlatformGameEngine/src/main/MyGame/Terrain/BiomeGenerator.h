#pragma once

#include "TerrainGenerator.h"

struct BiomeCell
{
    glm::ivec2 mCellId;
    glm::vec2 mCellPosition;

    float mDistance;
    float mWeight;
};

struct BiomeSample
{
    std::array<BiomeCell, 25> mCells{};
    uint32_t mCellCount = 0;
};

enum class BiomeType : uint32_t
{
    Plains,
    Forest,
    Desert,
    Mountains,
    Tundra,
    Count
};

struct BiomeDefinition {
    float mHeightScale;
    float mFrequencyScale;
};

class BiomeGenerator
{
public:
    static BiomeSample sample(const float worldX, const float worldZ, const float frequency, const uint32_t seed);
    static BiomeType getBiomeType(const glm::ivec2 cellId, const uint32_t seed);
    static BiomeDefinition getBiomeDefinition(const BiomeType type);

private:
    static BiomeSample noise(const float x, const float y, const uint32_t seed);
    static uint32_t hash(const int32_t x, const int32_t y, const uint32_t seed);
};