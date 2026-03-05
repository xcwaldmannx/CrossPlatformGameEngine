#pragma once

#include "../GpuFramePass.h"

#include <array>

namespace ascen
{

    struct ComputeGpuFramePass : GpuFramePass
    {
        ComputeGpuFramePass(
            const FramePassType type,
            const std::vector<VkDescriptorSet> descriptorSets,
            const VkPipeline pipeline,
            const VkPipelineLayout pipelineLayout,
            const std::vector<std::string> readBuffers,
            const std::vector<std::string> writeBuffers,
            const std::vector<VkImageView> readTextures,
            const std::vector<VkImageView> writeTextures,
            const std::array<uint32_t,3> groups);

        const std::array<uint32_t, 3> mGroups;
    };

}