#pragma once

#include <cstdint>

struct TerrainComponent
{
    uint32_t mSeed = 0;

    uint32_t mChunkSize = 1024;
    uint32_t mChunkHeight = 1024;
    uint32_t mChunkResolution = 32;
    uint32_t mMaxLod = 4;

    bool mIsActive = false;
};