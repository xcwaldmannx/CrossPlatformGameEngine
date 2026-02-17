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
	mMemoryFlags(memoryFlags)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = static_cast<VkDeviceSize>(mItemCount * mItemSize);
	bufferInfo.usage = usageFlags;
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
}

void Buffer::create(VkDevice device)
{
	// TODO: remove
}

void Buffer::destroy(VkDevice device)
{
	vkDestroyBuffer(device, mHandle, nullptr);
	vkFreeMemory(device, mMemory, nullptr);
}

void Buffer::update(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	const void* items,
	uint32_t itemCount,
	uint32_t itemSize,
	uint32_t offset)
{

	if (mMemoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		size_t size = static_cast<size_t>(itemCount * itemSize);
		size_t itemOffset = static_cast<size_t>(itemSize * offset);

		memcpy(static_cast<uint8_t*>(mMappedMemory) + itemOffset, items, size);
	}
	else
	{
		Buffer stagingBuffer(
			physicalDevice,
			device,
			itemCount,
			itemSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkDeviceSize sizeBytes = itemCount * itemSize;

		void* data = nullptr;
		vkMapMemory(device, stagingBuffer.mMemory, 0, sizeBytes, 0, &data);
		memcpy(data, items, sizeBytes);
		vkUnmapMemory(device, stagingBuffer.mMemory);

		copy(device, queue, commandPool, stagingBuffer, *this, false);

		stagingBuffer.destroy(device);
	}
}

void Buffer::copy(
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	Buffer& src,
	Buffer& dest,
	bool insertBarrier)
{
	VkCommandBuffer commandBuffer = commandPool->beginSingle(device);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = src.mItemCount * src.mItemSize;
	vkCmdCopyBuffer(commandBuffer, src.mHandle, dest.mHandle, 1, &copyRegion);

	if (insertBarrier)
	{
		Barrier::buffer(
			commandBuffer,
			dest.mHandle,
			VK_ACCESS_2_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_2_TRANSFER_BIT,
			VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT,
			VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT |
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
	}

	commandPool->endSingle(device, queue, commandBuffer);
}

Buffer::Memory Buffer::getMemoryInfo(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkBuffer buffer)
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
