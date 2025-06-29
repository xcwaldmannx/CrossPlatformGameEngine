#include "MemoryAllocator.h"

#include <stdexcept>

using namespace vulkan;

MemoryAllocator::MemoryAllocator() {}

MemoryAllocator::MemoryAllocator(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	: mPhysicalDevice(physicalDevice), mLogicalDevice(logicalDevice) {}

void MemoryAllocator::init(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice) {
	mPhysicalDevice = physicalDevice;
	mLogicalDevice = logicalDevice;
}

void MemoryAllocator::allocate(Buffer& buffer, VkMemoryPropertyFlags properties) {
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(mLogicalDevice, buffer.mBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(mLogicalDevice, &allocInfo, nullptr, &buffer.mMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(mLogicalDevice, buffer.mBuffer, buffer.mMemory, 0);
}

uint32_t MemoryAllocator::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
