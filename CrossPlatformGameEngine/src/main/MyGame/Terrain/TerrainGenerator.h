#pragma once

#include <glm/glm.hpp>

constexpr uint32_t TERRAIN_CHUNK_SIZE = 1024;
constexpr uint32_t TERRAIN_CHUNK_HEIGHT = 512;
constexpr uint32_t TERRAIN_CHUNK_RESOLUTION = 32;

struct TerrainVertex
{
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mTexCoords;
};

struct TerrainData
{
    std::vector<TerrainVertex> mVertices;
    std::vector<uint32_t> mIndices;
};

struct TerrainNeighborLods
{
    uint32_t mLeft;
    uint32_t mRight;
    uint32_t mDown;
    uint32_t mUp;
};

class TerrainGenerator
{
public:
    TerrainGenerator();

    static void generate(
        glm::ivec2 location,
        const uint32_t size,
        const uint32_t height,
        const uint32_t resolution,
        const uint32_t lod,
        const float frequency,
        const float uv,
        const TerrainNeighborLods& neighbors,
        TerrainData& data);

private:
    static float noise(const float x, const float y, const uint32_t seed);
    static uint32_t hash(const int32_t x, const int32_t y, const uint32_t seed);




    static void stitchEdge(
        std::vector<TerrainVertex>& vertices,
        const uint32_t lodResolution,
        const uint32_t neighborResolution,
        const uint32_t width,
        const bool alongX,
        const bool positiveSide);
};