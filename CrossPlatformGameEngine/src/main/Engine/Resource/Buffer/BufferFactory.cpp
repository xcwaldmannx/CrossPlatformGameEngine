#include "BufferFactory.h"

#include "Buffer.h"

using namespace ascen;

BufferFactory::BufferFactory(
	VkPhysicalDevice physicalDevice,
	VkDevice device) : mPhysicalDevice(physicalDevice), mDevice(device) {}

BufferPtr BufferFactory::create(
	uint32_t itemCount,
	uint32_t itemSize,
	BufferUsageFlags usageFlags,
	BufferMemoryFlags memoryFlags) const
{
	return std::make_shared<Buffer>(
		mPhysicalDevice,
		mDevice,
		itemCount,
		itemSize,
		usageFlags,
		memoryFlags);
}
