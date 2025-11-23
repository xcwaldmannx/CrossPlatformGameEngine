#include "IndexBuffer.h"

using namespace ascen;

IndexBuffer::IndexBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	const CommandPoolPtr& commandPool,
	uint32_t itemCount,
	uint32_t itemSize) :
	Buffer2(
		physicalDevice,
		device,
		itemCount,
		itemSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}