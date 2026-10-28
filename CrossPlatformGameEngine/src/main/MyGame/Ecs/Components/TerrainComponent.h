#pragma once

struct TerrainComponent
{
    uint32_t mSeed = 0;

    uint32_t mChunkSize = 1024;
    uint32_t mChunkHeight = 1024;
    uint32_t mChunkResolution = 32;

    bool mIsActive = false;
};