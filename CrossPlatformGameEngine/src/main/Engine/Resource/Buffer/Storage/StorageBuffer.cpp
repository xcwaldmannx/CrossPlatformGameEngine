#include "StorageBuffer.h"

using namespace ascen;

StorageBuffer::StorageBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	const CommandPoolPtr& commandPool,
	uint64_t itemCount,
	uint64_t itemSize) :
	Buffer(
		physicalDevice,
		device,
		itemCount,
		itemSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}
