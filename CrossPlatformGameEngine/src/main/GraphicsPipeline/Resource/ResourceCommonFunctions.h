#pragma once

#include "ResourceCommonTypes.h"

#ifndef INCLUDE_COMMAND_HANDLER_H
#define INCLUDE_COMMAND_HANDLER_H
#include "../CommandHandler/CommandHandler.h"
#endif

#include <stdexcept>

#include <vector>

#include <Vulkan/vulkan.h>

namespace ascen {

	uint32_t findMemoryType(
		VkPhysicalDevice physicalDevice,
		uint32_t typeFilter,
		VkMemoryPropertyFlags properties);

	void allocateBuffer(
		VkPhysicalDevice physicalDevice,
		VkDevice logicalDevice,
		VkMemoryPropertyFlags properties,
		Buffer* buffer);

	void createBuffer(
		VkPhysicalDevice physicalDevice,
		VkDevice logicalDevice,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		Buffer* buffer);

	void copyBuffer(
		LogicalDevice& logicalDevice,
		VkQueue graphicsQueue,
		CommandPool& commandPool,
		Buffer& srcBuffer,
		Buffer& dstBuffer,
		VkDeviceSize size,
		bool insertBarrier);

	void destroyBuffer(LogicalDevice& logicalDevice, Buffer& buffer);

	template<typename T>
	void copyStorageBuffer(
		PhysicalDevice& physicalDevice,
		LogicalDevice& logicalDevice,
		VkQueue graphicsQueue,
		CommandPool& commandPool,
		std::vector<T>& data,
		Buffer& buffer,
		bool insertBarrier)
	{
		size_t totalSize = sizeof(T) * data.size();

		Buffer stagingBuffer;

		createBuffer(
			physicalDevice.mDevice,
			logicalDevice.mDevice,
			totalSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer);

		void* mem;
		vkMapMemory(logicalDevice.mDevice, stagingBuffer.mMemory, 0, totalSize, 0, &mem);
		std::memcpy(mem, data.data(), totalSize);
		vkUnmapMemory(logicalDevice.mDevice, stagingBuffer.mMemory);

		copyBuffer(logicalDevice, graphicsQueue, commandPool, stagingBuffer, buffer, totalSize, insertBarrier);

		destroyBuffer(logicalDevice, stagingBuffer);
	}

}