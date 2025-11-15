#include "BufferFactory.h"

using namespace ascen;

BufferFactory::BufferFactory(
	VkPhysicalDevice physicalDevice,
	VkDevice device) : mPhysicalDevice(physicalDevice), mDevice(device) {}

BufferPtr BufferFactory::createVertex(
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<float>& vertices) const
{
	BufferPtr ptr = std::make_shared<Buffer>(
		Buffer::createVertexBuffer(
			mPhysicalDevice,
			mDevice,
			queueFamilyIndex,
			commandPool,
			vertices));
	return ptr;
}

BufferPtr BufferFactory::createIndirect(
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<Buffer::DrawCommand>& drawCommands) const
{
	BufferPtr ptr = std::make_shared<Buffer>(
		Buffer::createIndirectBuffer(
			mPhysicalDevice,
			mDevice,
			queueFamilyIndex,
			commandPool,
			drawCommands));
	return ptr;
}
