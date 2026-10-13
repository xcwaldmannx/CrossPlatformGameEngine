#pragma once

#include <vulkan/vulkan.h>

namespace ascen
{

	class Barrier
	{
	public:
		static void memory(
			VkCommandBuffer commandBuffer,
			VkAccessFlags2 srcAccess,
			VkPipelineStageFlags2 srcStage,
			VkAccessFlags2 dstAccess,
			VkPipelineStageFlags2 dstStage);

		static void buffer(
			VkCommandBuffer commandBuffer,
			VkBuffer buffer,
			VkAccessFlags2 srcAccess,
			VkPipelineStageFlags2 srcStage,
			VkAccessFlags2 dstAccess,
			VkPipelineStageFlags2 dstStage,
			VkDeviceSize size = VK_WHOLE_SIZE);

		static void image(
			VkCommandBuffer commandBuffer,
			VkImage image,
			uint32_t layers,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkAccessFlags2 srcAccess,
			VkPipelineStageFlags2 srcStage,
			VkAccessFlags2 dstAccess,
			VkPipelineStageFlags2 dstStage,
			VkImageAspectFlags aspectFlags);

	};

}
