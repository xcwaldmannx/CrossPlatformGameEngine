#pragma once

#include "../FramePass.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class GraphicsFramePass : public FramePass
	{
	public:
		GraphicsFramePass(
			const FramePassType type,
			const std::vector<VkDescriptorSet> descriptorSets,
			const VkPipeline pipeline,
			const VkPipelineLayout pipelineLayout,
			const std::vector<VkBuffer> readBuffers,
			const std::vector<VkBuffer> writeBuffers,
			const std::vector<VkImageView> readTextures,
			const std::vector<VkImageView> writeTextures,
			const std::vector<VkBuffer> vertexBuffers,
			const VkBuffer indexBuffer);

		const std::vector<VkBuffer> mVertexBuffers;
		const VkBuffer mIndexBuffer = VK_NULL_HANDLE;
	};

}