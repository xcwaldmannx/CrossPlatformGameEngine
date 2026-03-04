#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{
    struct GraphicsGpuFramePass;

    class LineCommandRecorder
    {
    public:
        void record(
            VkCommandBuffer commandBuffer,
            const GraphicsGpuFramePass* framePass,
            uint32_t frameIndex,
            VkBuffer indirectBuffer,
            uint32_t drawCommandCount);
    };

}