#include "UniformBuffer.h"

using namespace ascen;

UniformBuffer::UniformBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	uint64_t itemCount,
	uint64_t itemSize) :
	Buffer(
		physicalDevice,
		device,
		itemCount,
		itemSize,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{
	VkDeviceSize sizeBytes = static_cast<VkDeviceSize>(itemCount * itemSize);
	vkMapMemory(device, mMemory, 0, sizeBytes, 0, &mMappedMemory);
}
