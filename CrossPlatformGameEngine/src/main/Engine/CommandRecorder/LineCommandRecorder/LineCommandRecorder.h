#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{
    struct GraphicsGpuFramePass;

    class LineCommandRecorder : public CommandRecorder_I
    {
    public:
        void record(
            const VkCommandBuffer commandBuffer,
            const GraphicsPipelinePtr& pipeline,
            const std::vector<VkDescriptorSet> descriptorSets,
            const std::vector<VkBuffer> vertexBuffers,
            const VkBuffer indexBuffer);
    };

}