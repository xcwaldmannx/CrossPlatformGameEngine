#pragma once

#include <vulkan/vulkan.h>

namespace ascen
{

	class Barrier
	{
	public:
		static void buffer(
			VkCommandBuffer commandBuffer,
			VkBuffer buffer,
			VkAccessFlags srcAccess,
			VkAccessFlags dstAccess,
			VkPipelineStageFlags srcStage,
			VkPipelineStageFlags dstStage,
			VkDeviceSize size = VK_WHOLE_SIZE);

		static void image(
			VkCommandBuffer commandBuffer,
			VkImage image,
			uint32_t layers,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkPipelineStageFlags srcStage,
			VkPipelineStageFlags dstStage,
			VkAccessFlags srcAccess,
			VkAccessFlags dstAccess,
			VkImageAspectFlags aspectFlags);

	private:
	};

}
