#pragma once

#include "../../Engine/Core/Engine.h"

class MyPipeline
{
public:
    MyPipeline(ascen::Engine& engine);

    void init();

private:
    void initResources();
    void initStages();
    void initFramePasses();

private:
    ascen::Engine& mEngine;

    uint64_t mVertexTriangles = 0;
    uint64_t mVertexPoint = 0;

    uint64_t mBufferVertex = 0;
    uint64_t mBufferIndex = 0;
    uint64_t mBufferIndirect = 0;
    uint32_t mBufferIndirectSize = 1'000'000;

    uint64_t mBufferEntity = 0;
    uint64_t mBufferVertexBounds = 0;
    uint64_t mBufferIndexBounds = 0;

    uint64_t mSampler = 0;
    uint64_t mTexture = 0;

    uint64_t mDescriptorPoolCompute = 0;
    uint64_t mDescriptorSetLayoutCompute = 0;
    uint64_t mDescriptorSetCompute = 0;

    uint64_t mDescriptorPoolGraphics = 0;
    uint64_t mDescriptorSetLayoutGraphics = 0;
    uint64_t mDescriptorSetGraphics = 0;

    uint64_t mRenderPass = 0;
    uint64_t mRenderTarget = 0;
    uint64_t mFrameBuffer = 0;

    uint64_t mPipelineCompute = 0;
    uint64_t mPipelineGraphicsTriangles = 0;
    uint64_t mPipelineGraphicsBoundingBoxes = 0;
};