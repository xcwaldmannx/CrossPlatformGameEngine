#include "Buffer.h"

#include "../../Device/Physical/PhysicalDevice.h"
#include "../Barrier/Barrier.h"

#include <stdexcept>
#include <string.h>

using namespace ascen;

Buffer::Buffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	uint64_t itemCount,
	uint64_t itemSize,
	VkBufferUsageFlags usageFlags,
	VkMemoryPropertyFlags memoryFlags) :
	mItemCount(itemCount),
	mItemSize(itemSize),
	mUsageFlags(usageFlags),
	mMemoryFlags(memoryFlags)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = static_cast<VkDeviceSize>(mItemCount * mItemSize);
	bufferInfo.usage = mUsageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &mHandle) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	Memory mem = getMemoryInfo(physicalDevice, device, mHandle);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = mem.mSize;
	allocInfo.memoryTypeIndex = mem.mTypeIndex;

	if (vkAllocateMemory(device, &allocInfo, nullptr, &mMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, mHandle, mMemory, 0);

	if ((mUsageFlags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) &&
		(mMemoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
	{
		const VkDeviceSize sizeBytes = itemCount * itemSize;
		vkMapMemory(device, mMemory, 0, sizeBytes, 0, &mMappedMemory);
	}
}

void Buffer::destroy(VkDevice device)
{
	vkDestroyBuffer(device, mHandle, nullptr);
	vkFreeMemory(device, mMemory, nullptr);
}

void Buffer::upload(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	const void* items,
	uint32_t itemCount,
	uint32_t itemSize,
	uint32_t offset) const
{

	if (mMemoryFlags & BUFFER_MEMORY_HOST)
	{
		const auto size = itemCount * itemSize;
		const auto itemOffset = itemSize * offset;

		memcpy(static_cast<uint8_t*>(mMappedMemory) + itemOffset, items, size);
	}
	else
	{
		Buffer stagingBuffer(
			physicalDevice,
			device,
			itemCount,
			itemSize,
			BUFFER_USAGE_TRANSFER_SRC,
			BUFFER_MEMORY_HOST);

		const VkDeviceSize sizeBytes = itemCount * itemSize;

		void* data = nullptr;
		vkMapMemory(device, stagingBuffer.mMemory, 0, sizeBytes, 0, &data);
		memcpy(data, items, sizeBytes);
		vkUnmapMemory(device, stagingBuffer.mMemory);

		VkCommandBuffer commandBuffer = commandPool->beginSingle(device);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = itemCount * itemSize;
		vkCmdCopyBuffer(commandBuffer, stagingBuffer.mHandle, mHandle, 1, &copyRegion);

		commandPool->endSingle(device, queue, commandBuffer);

		stagingBuffer.destroy(device);
	}
}

void Buffer::download(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	void* items,
	uint32_t itemCount,
	uint32_t itemSize) const
{
	if (!(mMemoryFlags & BUFFER_MEMORY_LOCAL))
	{
		throw std::runtime_error("Cannot copy from CPU for a non-device-local buffer.");
	}

	if (!(mUsageFlags & BUFFER_USAGE_TRANSFER_SRC))
	{
		throw std::runtime_error("Cannot copy from a non-copyable source.");
	}

	Buffer stagingBuffer(
		physicalDevice,
		device,
		itemCount,
		itemSize,
		BUFFER_USAGE_TRANSFER_DST,
		BUFFER_MEMORY_HOST);

	const VkDeviceSize sizeBytes = itemCount * itemSize;

	VkCommandBuffer commandBuffer = commandPool->beginSingle(device);

	// TODO: make this more flexible
	Barrier::buffer(
	commandBuffer,
	mHandle,
	VK_ACCESS_2_SHADER_WRITE_BIT,
	VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
	VK_ACCESS_2_TRANSFER_READ_BIT,
	VK_PIPELINE_STAGE_2_TRANSFER_BIT);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = sizeBytes;
	vkCmdCopyBuffer(commandBuffer, mHandle, stagingBuffer.mHandle, 1, &copyRegion);

	commandPool->endSingle(device, queue, commandBuffer);

	void* data = nullptr;
	vkMapMemory(device, stagingBuffer.mMemory, 0, sizeBytes, 0, &data);
	memcpy(items, data, sizeBytes);
	vkUnmapMemory(device, stagingBuffer.mMemory);

	stagingBuffer.destroy(device);
}

size_t Buffer::getItemCount() const
{
	return mItemCount;
}

size_t Buffer::getItemSize() const
{
	return mItemSize;
}

Buffer::Memory Buffer::getMemoryInfo(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkBuffer buffer) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	Memory mem{};

	mem.mSize = memRequirements.size;

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & mMemoryFlags) == mMemoryFlags)
		{
			mem.mTypeIndex = i;
			break;
		}
	}

	return mem;
}
