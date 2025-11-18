#include "VertexBuffer.h"

using namespace ascen;

VertexBuffer::VertexBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<float>& vertices) :
	Buffer2(
		physicalDevice,
		device,
		vertices.size(),
		sizeof(float),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
	update<float>(physicalDevice, device, queueFamilyIndex, commandPool, vertices);
}