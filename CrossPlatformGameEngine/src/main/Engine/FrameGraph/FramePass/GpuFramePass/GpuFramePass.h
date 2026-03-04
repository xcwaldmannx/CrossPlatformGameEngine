#pragma once

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
            const std::vector<VkBuffer> readBuffers,
            const std::vector<VkBuffer> writeBuffers,
            const std::vector<VkImageView> readTextures,
            const std::vector<VkImageView> writeTextures);

        const std::vector<VkDescriptorSet> mDescriptorSets;
        const VkPipeline mPipeline;
        const VkPipelineLayout mPipelineLayout;
        const std::vector<VkBuffer> mReadBuffers;
        const std::vector<VkBuffer> mWriteBuffers;
        const std::vector<VkImageView> mReadTextures;
        const std::vector<VkImageView> mWriteTextures;
    };

}
