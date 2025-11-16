#pragma once

#include "Buffer_I.h"

#include "../../QueueFamilies/QueueFamilies.h"
#include "../../CommandPool/CommandPool.h"

namespace ascen
{

	class Buffer2 : public Buffer_I
	{
	public:
		struct Memory
		{
			VkDeviceSize mSize;
			uint32_t mTypeIndex;
		};

		Buffer2(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint64_t itemCount,
			uint64_t itemSize,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags);

		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

		template<typename T>
		void update(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			Buffer2& dst,
			const std::vector<T>& items)
		{
			Buffer2 stagingBuffer(
				physicalDevice,
				device,
				items.size(),
				sizeof(T),
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			VkDeviceSize sizeBytes = items.size() * sizeof(T);

			void* data = nullptr;
			vkMapMemory(device, stagingBuffer.mMemory, 0, sizeBytes, 0, &data);
			std::memcpy(data, items.data(), sizeBytes);
			vkUnmapMemory(device, stagingBuffer.mMemory);

			VkQueue queue = QueueFamilies::getDeviceQueue(device, queueFamilyIndex);
			copy(device, queue, commandPool, stagingBuffer, dst, false);

			stagingBuffer.destroy(device);
		}

	private:
		void copy(
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			Buffer2& src,
			Buffer2& dest,
			bool insertBarrier);

		Memory getMemoryInfo(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkBuffer buffer,
			VkMemoryPropertyFlags memoryFlags);

		size_t mItemCount = 0;      // number of items
		size_t mItemSize = 0;       // byte size of item
	};

}