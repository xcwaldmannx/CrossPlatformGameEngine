#include "BufferFactory.h"

#include "Vertex/VertexBuffer.h"
#include "Index/IndexBuffer.h"
#include "Uniform/UniformBuffer.h"
#include "Storage/StorageBuffer.h"
#include "Indirect/IndirectBuffer.h"

using namespace ascen;

BufferFactory::BufferFactory(
	VkPhysicalDevice physicalDevice,
	VkDevice device) : mPhysicalDevice(physicalDevice), mDevice(device) {}

BufferPtr BufferFactory::createVertex(
	const CommandPoolPtr& commandPool,
	uint32_t itemCount,
	uint32_t itemSize) const
{
	return std::make_shared<VertexBuffer>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		itemCount,
		itemSize);
}

BufferPtr BufferFactory::createIndex(
	const CommandPoolPtr& commandPool,
	uint32_t itemCount,
	uint32_t itemSize) const
{
	return std::make_shared<IndexBuffer>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		itemCount,
		itemSize);
}

BufferPtr BufferFactory::createUniform(
	uint32_t itemCount,
	uint32_t itemSize) const
{
	return std::make_shared<UniformBuffer>(
		mPhysicalDevice,
		mDevice,
		itemCount,
		itemSize);
}

BufferPtr BufferFactory::createStorage(
	const CommandPoolPtr& commandPool,
	uint32_t itemCount,
	uint32_t itemSize) const
{
	return std::make_shared<StorageBuffer>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		itemCount,
		itemSize);
}

BufferPtr BufferFactory::createIndirect(
	const CommandPoolPtr& commandPool,
	uint32_t itemCount,
	uint32_t itemSize) const
{
	return std::make_shared<IndirectBuffer>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		itemCount,
		itemSize);
}
