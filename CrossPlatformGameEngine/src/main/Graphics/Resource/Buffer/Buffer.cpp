#include "Buffer.h"

#include "../../CommandPool/CommandPool.h"
#include "../Barrier/Barrier.h"

#include <stdexcept>

using namespace ascen;

void Buffer::copy(
	VkDevice device,
	VkQueue graphicsQueue,
	const std::shared_ptr<CommandPool>& commandPool,
	Buffer& src,
	Buffer& dest,
	bool insertBarrier)
{
	VkCommandBuffer commandBuffer = commandPool->beginSingle(device);
	

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = src.mItemCount * src.mItemSize;
	vkCmdCopyBuffer(commandBuffer, src.mBuffer, dest.mBuffer, 1, &copyRegion);

	if (insertBarrier)
	{
		Barrier::buffer(
		commandBuffer,
		dest.mBuffer,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | 
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT |
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
	}

	commandPool->endSingle(device, graphicsQueue, commandBuffer);
}

void Buffer::destroy(VkDevice device, Buffer& buffer)
{
	vkDestroyBuffer(device, buffer.mBuffer, nullptr);
	vkFreeMemory(device, buffer.mMemory, nullptr);
}

BufferMemory Buffer::getMemoryInfo(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkBuffer buffer,
	VkMemoryPropertyFlags memoryFlags)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	BufferMemory mem{};

	mem.mSize = memRequirements.size;

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags)
		{
			mem.mTypeIndex = i;
			break;
		}
	}

	return mem;
}

VkBuffer Buffer::getBuffer() const
{
	return mBuffer;
}

VkDeviceMemory Buffer::getMemory() const
{
	return mMemory;
}

void* Buffer::getMappedMemory() const
{
	return mMappedMemory;
}

Buffer::Buffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	size_t itemCount,
	size_t itemSize,
	VkBufferUsageFlags usageFlags,
	VkMemoryPropertyFlags memoryFlags)
	: mItemCount(itemCount), mItemSize(itemSize)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = itemCount * itemSize;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &mBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	BufferMemory mem = getMemoryInfo(physicalDevice, device, mBuffer, memoryFlags);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = mem.mSize;
	allocInfo.memoryTypeIndex = mem.mTypeIndex;

	if (vkAllocateMemory(device, &allocInfo, nullptr, &mMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, mBuffer, mMemory, 0);
}
