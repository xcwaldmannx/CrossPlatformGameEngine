#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{

    class ComputeCommandRecorder : public CommandRecorder_I
    {
    public:
        ComputeCommandRecorder(
            PipelineRegistry& pipelineRegistry,
            DescriptorRegistry& descriptorRegistry,
            ResourceRegistry& resourceRegistry);

        void record(
            VkCommandBuffer commandBuffer,
            const ComputeGpuFramePass* framePass,
            uint32_t frameIndex);
    };

}
