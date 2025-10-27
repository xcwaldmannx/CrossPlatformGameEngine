#include "ResourceCommonFunctions.h"

namespace ascen {

	uint32_t findMemoryType(
		VkPhysicalDevice physicalDevice,
		uint32_t typeFilter,
		VkMemoryPropertyFlags properties) {

		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void allocateBuffer(
		VkPhysicalDevice physicalDevice,
		VkDevice logicalDevice,
		VkMemoryPropertyFlags properties,
		Buffer* buffer) {

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice, buffer->mBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &buffer->mMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(logicalDevice, buffer->mBuffer, buffer->mMemory, 0);
	}

	void createBuffer(
		VkPhysicalDevice physicalDevice,
		VkDevice logicalDevice,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		Buffer* buffer) {

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer->mBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		allocateBuffer(physicalDevice, logicalDevice, properties, buffer);
	}

	void copyBuffer(
		LogicalDevice& logicalDevice,
		VkQueue graphicsQueue,
		CommandPool& commandPool,
		Buffer& srcBuffer,
		Buffer& dstBuffer,
		VkDeviceSize size,
		bool insertBarrier)
	{

		VkCommandBuffer commandBuffer = nullptr;
		beginSingleTimeCommands(logicalDevice, commandPool, &commandBuffer);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer.mBuffer, dstBuffer.mBuffer, 1, &copyRegion);

		if (insertBarrier) {
			VkBufferMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.buffer = dstBuffer.mBuffer;
			barrier.offset = 0;
			barrier.size = VK_WHOLE_SIZE;

			vkCmdPipelineBarrier(
				commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				0,
				0, nullptr,
				1, &barrier,
				0, nullptr
			);
		}

		endSingleTimeCommands(logicalDevice, graphicsQueue, commandPool, &commandBuffer);
	}

	void destroyBuffer(LogicalDevice& logicalDevice, Buffer& buffer) {
		vkDestroyBuffer(logicalDevice.mDevice, buffer.mBuffer, nullptr);
		vkFreeMemory(logicalDevice.mDevice, buffer.mMemory, nullptr);
	}

}