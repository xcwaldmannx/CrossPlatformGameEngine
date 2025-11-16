#include "IndirectBuffer.h"

using namespace ascen;

IndirectBuffer::IndirectBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<DrawCommand>& drawCommands) :
	Buffer2(
		physicalDevice,
		device,
		drawCommands.size(),
		sizeof(DrawCommand),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
	update<DrawCommand>(physicalDevice, device, queueFamilyIndex, commandPool, *this, drawCommands);
}