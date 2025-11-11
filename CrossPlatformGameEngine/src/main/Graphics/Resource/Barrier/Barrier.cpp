#include "Barrier.h"

using namespace ascen;

void Barrier::buffer(
	VkCommandBuffer commandBuffer,
	VkBuffer buffer,
	VkAccessFlags srcAccess,
	VkAccessFlags dstAccess,
	VkPipelineStageFlags srcStage,
	VkPipelineStageFlags dstStage,
	VkDeviceSize size)
{
	VkBufferMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier.srcAccessMask = srcAccess;
	barrier.dstAccessMask = dstAccess;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.buffer = buffer;
	barrier.offset = 0;
	barrier.size = size;

	vkCmdPipelineBarrier(
		commandBuffer,
		srcStage,
		dstStage,
		0,
		0, nullptr,
		1, &barrier,
		0, nullptr);
}

void Barrier::image(
	VkCommandBuffer commandBuffer,
	VkImage image,
	uint32_t layers,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkPipelineStageFlags srcStage,
	VkPipelineStageFlags dstStage,
	VkAccessFlags srcAccess,
	VkAccessFlags dstAccess,
	VkImageAspectFlags aspectFlags)
{
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = aspectFlags;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layers;

	vkCmdPipelineBarrier(
		commandBuffer,
		srcStage, dstStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
}
