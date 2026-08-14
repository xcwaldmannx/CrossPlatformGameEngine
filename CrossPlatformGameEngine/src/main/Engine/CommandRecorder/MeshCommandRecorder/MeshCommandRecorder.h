#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{

    class GraphicsGpuFramePass;

    class MeshCommandRecorder : public CommandRecorder_I
    {
    public:
        void record(
        const VkCommandBuffer commandBuffer,
        const GraphicsPipelinePtr& pipeline,
        const std::vector<VkDescriptorSet> descriptorSets,
        const std::vector<VkBuffer> vertexBuffers,
        const VkBuffer indexBuffer,
        const VkBuffer indirectBuffer,
        const uint32_t indirectCount);
    };

}