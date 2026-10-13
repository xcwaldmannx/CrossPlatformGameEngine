#include "Barrier.h"

using namespace ascen;

void Barrier::memory(
	VkCommandBuffer commandBuffer,
	VkAccessFlags2 srcAccess,
	VkPipelineStageFlags2 srcStage,
	VkAccessFlags2 dstAccess,
	VkPipelineStageFlags2 dstStage)
{
	VkMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
	barrier.srcAccessMask = srcAccess;
	barrier.dstAccessMask = dstAccess;
	barrier.srcStageMask = srcStage;
	barrier.dstStageMask = dstStage;

	VkDependencyInfo dep{};
	dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dep.memoryBarrierCount = 1;
	dep.pMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(
		commandBuffer,
		&dep);
}

void Barrier::buffer(
	VkCommandBuffer commandBuffer,
	VkBuffer buffer,
	VkAccessFlags2 srcAccess,
	VkPipelineStageFlags2 srcStage,
	VkAccessFlags2 dstAccess,
	VkPipelineStageFlags2 dstStage,
	VkDeviceSize size)
{
	VkBufferMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;

	barrier.srcAccessMask = srcAccess;
	barrier.srcStageMask = srcStage;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.dstAccessMask = dstAccess;
	barrier.dstStageMask = dstStage;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.buffer = buffer;
	barrier.offset = 0;
	barrier.size = size;

	VkDependencyInfo dep{};
	dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dep.bufferMemoryBarrierCount = 1;
	dep.pBufferMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(
		commandBuffer,
		&dep);
}

void Barrier::image(
	VkCommandBuffer commandBuffer,
	VkImage image,
	uint32_t layers,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkAccessFlags2 srcAccess,
	VkPipelineStageFlags2 srcStage,
	VkAccessFlags2 dstAccess,
	VkPipelineStageFlags2 dstStage,
	VkImageAspectFlags aspectFlags)
{
	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	barrier.srcAccessMask = srcAccess;
	barrier.srcStageMask = srcStage;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.dstAccessMask = dstAccess;
	barrier.dstStageMask = dstStage;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;
	barrier.subresourceRange.aspectMask = aspectFlags;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layers;

	VkDependencyInfo dep{};
	dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dep.imageMemoryBarrierCount = 1;
	dep.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(
		commandBuffer,
		&dep);
}
