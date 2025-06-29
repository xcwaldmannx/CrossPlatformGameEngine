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

}