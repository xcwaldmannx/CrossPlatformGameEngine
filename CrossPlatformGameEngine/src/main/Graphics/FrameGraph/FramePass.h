#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class FramePass
	{
	public:
		FramePass(
			const std::vector<VkBuffer> vertexBuffers,
			const VkBuffer indexBuffer,
			const std::vector<VkBuffer> readBuffers,
			const std::vector<VkBuffer> writeBuffers,
			const std::vector<VkBuffer> readTextures,
			const std::vector<VkBuffer> writeTextures,
			const std::vector<VkDescriptorSet> descriptorSets,
			const VkPipeline pipeline,
			const VkPipelineLayout pipelineLayout);

		const std::vector<VkBuffer> mVertexBuffers;
		const VkBuffer mIndexBuffer = VK_NULL_HANDLE;
		const std::vector<VkBuffer> mReadBuffers;
		const std::vector<VkBuffer> mWriteBuffers;
		const std::vector<VkBuffer> mReadTextures;
		const std::vector<VkBuffer> mWriteTextures;
		const std::vector<VkDescriptorSet> mDescriptorSets;
		const VkPipeline mPipeline = VK_NULL_HANDLE;
		const VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
	};

}
