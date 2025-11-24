#pragma once

#include "FramePass_I.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class FramePass : public FramePass_I
	{
	public:
		FramePass(
			const FramePassType type,
			const std::vector<VkDescriptorSet> descriptorSets,
			const VkPipeline pipeline,
			const VkPipelineLayout pipelineLayout,
			const std::vector<VkBuffer> readBuffers,
			const std::vector<VkBuffer> writeBuffers,
			const std::vector<VkImageView> readTextures,
			const std::vector<VkImageView> writeTextures);

		const FramePassType mType;
		const std::vector<VkDescriptorSet> mDescriptorSets;
		const VkPipeline mPipeline;
		const VkPipelineLayout mPipelineLayout;
		const std::vector<VkBuffer> mReadBuffers;
		const std::vector<VkBuffer> mWriteBuffers;
		const std::vector<VkImageView> mReadTextures;
		const std::vector<VkImageView> mWriteTextures;
	};

}
