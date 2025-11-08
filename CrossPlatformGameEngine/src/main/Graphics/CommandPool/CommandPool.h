#pragma once

#include "../HandleManager/Handle.h"

#include "../Device/Physical/PhysicalDevice.h"
#include "../Swapchain/Swapchain.h"
#include "../RenderPass/RenderPass.h"
#include "../Pipeline/Pipeline.h"

#include <memory>

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

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
	public:
		CommandPool(uint32_t graphicsFamily);

		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

		void record(
			VkPhysicalDevice physicalDevice,
			uint32_t frameIndex,
			uint32_t imageIndex,
			VkDescriptorSet descriptorSet,
			VkBuffer vertexBuffer,
			VkBuffer indexBuffer,
			std::shared_ptr<RenderPass> renderPass,
			std::shared_ptr<Swapchain> swapchain,
			std::shared_ptr<Pipeline_I> pipeline,
			const CommandDrawData& data);

		void record(
			VkPhysicalDevice physicalDevice,
			uint32_t frameIndex,
			uint32_t imageIndex,
			VkDescriptorSet descriptorSet,
			VkBuffer vertexBuffer,
			VkBuffer indexBuffer,
			VkBuffer indirectBuffer,
			std::shared_ptr<RenderPass> renderPass,
			std::shared_ptr<Swapchain> swapchain,
			std::shared_ptr<Pipeline_I> pipeline,
			const std::vector<VkDrawIndexedIndirectCommand>& drawCommands);

		void beginSingleTimeCommands(
			VkDevice device,
			VkCommandBuffer* buffer);

		void endSingleTimeCommands(
			VkDevice device,
			VkQueue queue,
			VkCommandBuffer* buffer);

		const VkCommandBuffer* getBufferIndex(size_t index) const;

	protected:
		VkCommandPoolCreateInfo mCreateInfo{};
		VkCommandBufferAllocateInfo mAllocInfo{};

	private:
		std::vector<VkCommandBuffer> mCommandBuffers;
	};

}
