#pragma once

#include <string>

#include "../FramePass.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    struct GpuFramePass : FramePass
    {
        GpuFramePass(
            const FramePassType type,
            const std::vector<VkDescriptorSet> descriptorSets,
            const VkPipeline pipeline,
            const VkPipelineLayout pipelineLayout,
            const std::vector<std::string> readBuffers,
            const std::vector<std::string> writeBuffers,
            const std::vector<VkImageView> readTextures,
            const std::vector<VkImageView> writeTextures);

        const std::vector<VkDescriptorSet> mDescriptorSets;
        const VkPipeline mPipeline;
        const VkPipelineLayout mPipelineLayout;
        const std::vector<std::string> mReadBuffers;
        const std::vector<std::string> mWriteBuffers;
        const std::vector<VkImageView> mReadTextures;
        const std::vector<VkImageView> mWriteTextures;
    };

}
