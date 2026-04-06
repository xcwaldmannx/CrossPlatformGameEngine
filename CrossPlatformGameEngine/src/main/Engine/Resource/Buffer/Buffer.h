#pragma once

#include "Buffer_I.h"

#include "../../CommandPool/CommandPool.h"

namespace ascen
{

	class Buffer : public Buffer_I
	{
	public:
		struct Memory
		{
			VkDeviceSize mSize;
			uint32_t mTypeIndex;
		};

		Buffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint64_t itemCount,
			uint64_t itemSize,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags);

		void destroy(VkDevice device) override;

		void upload(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			const void* items,
			uint32_t itemCount,
			uint32_t itemSize,
			uint32_t offset = 0);

		void download(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			void* items,
			uint32_t itemCount,
			uint32_t itemSize);

		size_t getItemCount() const;

		size_t getItemSize() const;

	private:
		void copy(
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			Buffer& src,
			Buffer& dest,
			bool insertBarrier);

		Memory getMemoryInfo(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkBuffer buffer);

		size_t mItemCount = 0;      // number of items
		size_t mItemSize = 0;       // byte size of item
		VkBufferUsageFlags mUsageFlags;
		VkMemoryPropertyFlags mMemoryFlags;
	};

}