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
	return std::make_shared<VertexBuffer>(
		mPhysicalDevice,
		mDevice,
		queueFamilyIndex,
		commandPool,
		vertices);
}

BufferPtr BufferFactory::createIndex(
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<uint32_t>& indices) const
{
	return std::make_shared<IndexBuffer>(
		mPhysicalDevice,
		mDevice,
		queueFamilyIndex,
		commandPool,
		indices);
}

BufferPtr BufferFactory::createIndirect(
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<Buffer::DrawCommand>& drawCommands) const
{
	return std::make_shared<IndirectBuffer>(
		mPhysicalDevice,
		mDevice,
		queueFamilyIndex,
		commandPool,
		drawCommands);
}
