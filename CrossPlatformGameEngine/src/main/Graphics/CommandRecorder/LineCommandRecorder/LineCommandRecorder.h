#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{

    class GraphicsFramePass;

    class LineCommandRecorder
    {
    public:
        void record(
            VkCommandBuffer commandBuffer,
            const GraphicsFramePass* framePass,
            uint32_t frameIndex,
            VkBuffer indirectBuffer,
            uint32_t drawCommandCount);
    };

}