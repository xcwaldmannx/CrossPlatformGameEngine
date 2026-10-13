#pragma once

#include "../CommandRecorder_I.h"

namespace ascen
{

    class ComputeCommandRecorder : public CommandRecorder_I
    {
    public:
        void record(
        VkCommandBuffer commandBuffer,
        const ComputePipelinePtr& pipeline,
        const std::vector<VkDescriptorSet>& descriptorSets,
        const uint32_t computeGroups[3]);
    };

}
