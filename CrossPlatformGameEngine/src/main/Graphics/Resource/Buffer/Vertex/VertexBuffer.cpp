#include "VertexBuffer.h"

using namespace ascen;

VertexBuffer::VertexBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	const CommandPoolPtr& commandPool,
	uint32_t itemCount,
	uint32_t itemSize) :
	Buffer(
		physicalDevice,
		device,
		itemCount,
		itemSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}