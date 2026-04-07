#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{
    struct GraphicsGpuFramePass;

    class LineCommandRecorder : public CommandRecorder_I
    {
    public:
        LineCommandRecorder(
            PipelineRegistry& pipelineRegistry,
            DescriptorRegistry& descriptorRegistry,
            ResourceRegistry& resourceRegistry);

        void record(
            VkCommandBuffer commandBuffer,
            const GraphicsGpuFramePass* framePass,
            uint32_t frameIndex);
    };

}