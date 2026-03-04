#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{

    class GraphicsGpuFramePass;

    class MeshCommandRecorder
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