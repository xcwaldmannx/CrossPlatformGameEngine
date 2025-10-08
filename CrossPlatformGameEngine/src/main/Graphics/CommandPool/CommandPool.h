#pragma once

#include "../HandleManager/Handle.h"

#include "../Swapchain/Swapchain.h"
#include "../RenderPass/RenderPass.h"
#include "../Pipeline/Pipeline.h"

#include <memory>

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{
	
	class CommandPool : public Handle<VkCommandPool>
	{
	public:
		CommandPool(uint32_t graphicsFamily);

		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

		void record(
			uint32_t frameIndex,
			uint32_t imageIndex,
			VkDescriptorSet descriptorSet,
			VkBuffer vertexBuffer,
			VkBuffer indexBuffer,
			std::shared_ptr<RenderPass> renderPass,
			std::shared_ptr<Swapchain> swapchain,
			std::shared_ptr<Pipeline> pipeline);

		// TODO: complete these functions
		void beginSingleTimeCommands(
			VkDevice device,
			VkCommandBuffer* buffer);

		void endSingleTimeCommands(
			VkDevice device,
			VkQueue queue,
			VkCommandBuffer* buffer);

	protected:
		VkCommandPoolCreateInfo mCreateInfo{};
		VkCommandBufferAllocateInfo mAllocInfo{};

	private:
		std::vector<VkCommandBuffer> mCommandBuffers;
	};

}
