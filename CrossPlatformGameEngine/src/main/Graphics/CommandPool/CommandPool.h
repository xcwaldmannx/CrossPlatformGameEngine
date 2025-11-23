#pragma once

#include "../Types.h"
#include "../Handle/Handle.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class CommandPoolFactory;
	class RenderPass;
	class Swapchain;
	class FrameGraph;

	struct CommandDrawData
	{
		uint32_t mMeshCount = 0;
		std::vector<uint32_t> mVertexOffsets;
		std::vector<uint32_t> mIndexCounts;
		std::vector<uint32_t> mIndexOffsets;
		std::vector<uint32_t> mTransformOffsets;
	};
	
	class CommandPool : public Handle<VkCommandPool>
	{
	private:
		CommandPool(
			VkDevice device,
			uint32_t queueFamilyIndex);

	public:
		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

		void record(
			VkPhysicalDevice physicalDevice,
			uint32_t frameIndex,
			uint32_t imageIndex,
			const FrameGraph& frameGraph,
			VkBuffer indirectBuffer,
			const RenderPassPtr& renderPass,
			const SwapchainPtr& swapchain,
			uint32_t drawCommandCount);

		VkCommandBuffer beginSingle(VkDevice device);
		void endSingle(VkDevice device, VkQueue queue, VkCommandBuffer buffer);

		const VkCommandBuffer* getBufferIndex(size_t index) const;

	private:
		std::vector<VkCommandBuffer> mCommandBuffers;

		friend class CommandPoolFactory;
	};

}
