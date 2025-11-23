#pragma once

#include "Buffer_I.h"

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

		void update(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			const void* items,
			uint32_t itemCount,
			uint32_t itemSize);

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