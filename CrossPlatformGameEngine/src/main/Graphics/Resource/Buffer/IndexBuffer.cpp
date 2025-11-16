#include "IndexBuffer.h"

using namespace ascen;

IndexBuffer::IndexBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<uint32_t>& indices) :
	Buffer2(
		physicalDevice,
		device,
		indices.size(),
		sizeof(uint32_t),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
	update<uint32_t>(physicalDevice, device, queueFamilyIndex, commandPool, *this, indices);
}