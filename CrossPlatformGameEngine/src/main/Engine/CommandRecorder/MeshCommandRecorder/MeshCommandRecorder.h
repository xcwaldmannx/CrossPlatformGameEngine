#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{

    class GraphicsGpuFramePass;

    class MeshCommandRecorder : public CommandRecorder_I
    {
    public:
        MeshCommandRecorder(
            PipelineRegistry& pipelineRegistry,
            DescriptorRegistry& descriptorRegistry,
            ResourceRegistry& resourceRegistry);

        void record(
            VkCommandBuffer commandBuffer,
            const GraphicsGpuFramePass* framePass,
            uint32_t frameIndex,
            VkBuffer indirectBuffer,
            uint32_t drawCommandCount);
    };

}